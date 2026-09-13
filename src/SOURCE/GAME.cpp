#include <va.h>
#include <SOURCE/hero.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/philAI.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/GAME.h>
#include <BASE/message.h>
#include <BASE/Icon2b.h>
#include <BASE/Misc.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/FINDPATH.h>
#include <SOURCE/HERO.h>
#include <SOURCE/KB.h>
#include <SOURCE/REMOTE.h>
#include <io.h>
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
#include <SOURCE/kbwin.h>

#define GAME_SCORE_EXTRA_LARGE_DAY_SCALE 0.6
#define GAME_SCORE_LARGE_DAY_SCALE                                                 \
    0.8
#define GAME_SCORE_MEDIUM_DAY_SCALE 1.0
#define GAME_SCORE_SMALL_DAY_SCALE 1.4
#define GAME_VIEW_ARMY_FRAME_DELAY_SCALE 1.35
#define GAME_ULTIMATE_ARTIFACT_TWO_THIRDS                                          \
    0.66

H2_ENUM_BEGIN(MapTilesetConstant)
    WAGON_CAMP_ACTIVE_FRAME = 129
H2_ENUM_END(MapTilesetConstant)

H2_ENUM_BEGIN(ExpansionCampaignSaveConstant)
    CAMPAIGN_SAVE_PREFIX_SIZE = 0x4f
H2_ENUM_END(ExpansionCampaignSaveConstant)

H2_ENUM_BEGIN(GameSaveFormatConstant)
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
H2_ENUM_END(GameSaveFormatConstant)

H2_ENUM_BEGIN(GameHeroSelectionConstant)
    HERO_SELECTION_RETRY_LIMIT            = 2000,
    HERO_SELECTION_REUSE_RETRY_LIMIT      = 1500,
    HERO_SELECTION_FACTION_RETRY_LIMIT    = 100,
    HERO_SELECTION_EXPERIENCE_RETRY_LIMIT = 40,
    HERO_SELECTION_MINIMUM_EXPERIENCE     = 1000,
    HERO_SELECTION_CAMPAIGN_RETRY_LIMIT   = 500
H2_ENUM_END(GameHeroSelectionConstant)

H2_ENUM_BEGIN(GameInitialStateConstant)
    INITIAL_DIFFICULTY_RATING   = 1,
    INITIAL_PLAYER_COUNT        = 4,
    INITIAL_CALENDAR_VALUE      = 1,
    INITIAL_MAP_CHANGE_SEQUENCE = 1,
    INITIAL_RECORD_TYPE_STRIDE  = IDX(GAME_HERO_COUNT) / IDX(FACTION_COUNT),
    BOAT_HERO_NONE              = -1,
    BOAT_SLOT_EMPTY             = -1
H2_ENUM_END(GameInitialStateConstant)

H2_ENUM_BEGIN(NeutralTownReinforcementConstant)
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
H2_ENUM_END(NeutralTownReinforcementConstant)

H2_ENUM_BEGIN(NewMapConstant)
    VICTORY_SIDE_SEARCH_DONE                   = 99,
    STARTING_HERO_TOWN_PASS_COUNT              = 2,
    STARTING_HERO_ALLOW_NON_CASTLE_PASS        = 1,
    NECROMANCER_SHRINE_BUILD_MASK              = 1 << IDX(BUILDING_SLOT_NECROMANCER_SHRINE),
    CAMPAIGN_SCENARIO_NUMBER_OFFSET            = 1,
    CAMPAIGN_ROLAND_ULTIMATE_CROWN_SCENARIO    = 8,
    CAMPAIGN_ARCHIBALD_ULTIMATE_CROWN_SCENARIO = 9,
    ULTIMATE_DISTANCE_ROLL_MIN                 = 1,
    ULTIMATE_DISTANCE_COMMON_ROLL_MAX          = 20,
    ULTIMATE_DISTANCE_BONUS_ROLL_MAX           = 30,
    ULTIMATE_ARTIFACT_BORDER_MARGIN            = 9,
    ULTIMATE_HUMAN_DISTANCE_RETRY_LIMIT        = 200,
    ULTIMATE_SEARCH_REGION_RETRY_LIMIT         = 400
H2_ENUM_END(NewMapConstant)

H2_ENUM_BEGIN(GameDailyEconomyConstant)
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
    DAILY_RESOURCE_BONUS_LAST_DAY   = IDX(RES_GOLD),
    GAME_WEEKS_PER_MONTH            = 4,
    POWER_RING_DAILY_MANA_BONUS     = 2
H2_ENUM_END(GameDailyEconomyConstant)

H2_ENUM_BEGIN(WeeklyRuntimeConstant)
    WEEK_NAME_LAST                = KB_WEEK_NAME_COUNT - 1,
    CREATURE_WEEK_LAST            = IDX(CREATURE_BONE_DRAGON),
    SPECIAL_WEEK_ROLL_MAX         = 4,
    CASTLE_GROWTH_SPECIAL_BONUS   = 2,
    CASTLE_GROWTH_WELL_BONUS      = 8,
    NEUTRAL_CASTLE_GROWTH_DIVISOR = 2,
    CREATURE_WEEK_GROWTH_BONUS    = 5,
    CREATURE_MONTH_MULTIPLIER     = 2
H2_ENUM_END(WeeklyRuntimeConstant)

H2_ENUM_BEGIN(RandomMapConstant)
    RANDOM_TOWN_SCRATCH_WIDTH    = 2,
    RANDOM_MINE_RETRY_LIMIT      = 30,
    RANDOM_MINE_FOOTPRINT_WIDTH  = 2,
    RANDOM_MINE_RESOURCE_COUNT   = IDX(MINE_TYPE_GOLD) + 1,
    RANDOM_HERO_SECOND_SELECTION = 2,
    RANDOM_HERO_AVERAGE_DIVISOR  = 2
H2_ENUM_END(RandomMapConstant)

H2_ENUM_BEGIN(GameMapSetupConstant)
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
    TOWN_UPGRADE_BUILDING_FIRST           = IDX(BUILDING_SLOT_UPGRADE_FIRST),
    TOWN_UPGRADE_BUILDING_LAST            = IDX(BUILDING_SLOT_SPECIAL_THIRTY),
    TOWN_DWELLING_BUILDING_FIRST          = IDX(BUILDING_SLOT_DWELLING_FIRST),
    TOWN_DWELLING_BUILDING_LAST           = IDX(BUILDING_SLOT_DWELLING_LAST),
    TOWN_UPGRADE_TO_DWELLING_OFFSET       = 5,
    MAP_HERO_PROCESS_PASS_COUNT           = 3,
    MAP_HERO_ASSIGNMENT_PASS              = 0,
    MAP_HERO_CLASS_PASS                   = 1,
    MAP_HERO_PLACEMENT_PASS               = 2,
    MAP_HERO_FRAME_STRIDE                 = IDX(FACTION_NEUTRAL) + 1,
    MAP_HERO_RANDOM_FACTION_FRAME         = IDX(FACTION_NEUTRAL),
    MAP_HEROES_PER_FACTION                = GAME_HERO_COUNT / IDX(FACTION_COUNT),
    MAP_HERO_CLASS_SCAN_RETRY_LIMIT       = 1000,
    MAP_HERO_SCOUTING_SKILL_INDEX         = IDX(HERO_SKILL_SCOUTING),
    HERO_CONSISTENCY_PLAYABLE_FACTION_MAX = IDX(FACTION_NECROMANCER),
    HERO_CONSISTENCY_POOL_THRESHOLD       = 40
H2_ENUM_END(GameMapSetupConstant)

H2_ENUM_BEGIN(GamePlayerTurnConstant)
    GAME_AI_MUSIC_TRACK            = 28,
    ENVIRONMENT_ORIGIN_TILE_OFFSET = 7
H2_ENUM_END(GamePlayerTurnConstant)

H2_ENUM_BEGIN(GameScoreConstant)
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
H2_ENUM_END(GameScoreConstant)

H2_ENUM_BEGIN(GameJoinConstant)
    JOIN_HEADER_SIZE = 2,
    JOIN_BUFFER_SIZE = 700000
H2_ENUM_END(GameJoinConstant)

H2_ENUM_BEGIN(RemoteSaveConstant)
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
H2_ENUM_END(RemoteSaveConstant)

H2_ENUM_BEGIN(NewTurnConstant)
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
H2_ENUM_END(NewTurnConstant)

H2_ENUM_BEGIN(DiffRuntimeConstant)
    DIFF_WORD_SHIFT       = 16,
    DIFF_WORD_HEADER_SIZE = 3,
    DIFF_BYTE_SHIFT       = 8,
    DIFF_BYTE_HEADER_SIZE = 2,
    DIFF_BUFFER_EXTRA     = 5000,
    DIFF_MAX_SHORT_MATCH  = 3,
    DIFF_COPY_FLAG_SHIFT  = 7
H2_ENUM_END(DiffRuntimeConstant)

#define SCORE_SECOND_TIER_FACTOR 0.5
#define SCORE_SECOND_TIER_BASE_DEDUCTION 30.0
#define SCORE_THIRD_TIER_FACTOR 0.25
#define SCORE_THIRD_TIER_BASE_DEDUCTION 60.0
#define SCORE_FINAL_TIER_FACTOR 0.125

H2_ENUM_BEGIN(GameDiffEncoding)
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
H2_ENUM_END(GameDiffEncoding)

H2_ENUM_BEGIN(GameSaveSentinel)
    SAVED_TOWN_OFF_MAP = 0xff
H2_ENUM_END(GameSaveSentinel)

H2_ENUM_BEGIN(GameMonthlyConstant)
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
H2_ENUM_END(GameMonthlyConstant)

H2_ENUM_BEGIN(GameRandomArtifactConstant)
    ARTIFACT_BASE_TABLE_SIZE      = IDX(ARTIFACT_MAGIC_BOOK) + 1,
    ARTIFACT_FIRST                = IDX(ARTIFACT_ULTIMATE_BOOK),
    ARTIFACT_BASE_LAST            = IDX(ARTIFACT_MAGIC_BOOK),
    ARTIFACT_EXPANSION_LAST       = IDX(ARTIFACT_SPADE_NECROMANCY),
    ARTIFACT_UNIQUE_RETRIES       = 100,
    ARTIFACT_CURSED_ROLL_MAX      = 100,
    ARTIFACT_CURSED_REJECT_CHANCE = 30,
    ARTIFACT_ID_OFFSET            = 1
H2_ENUM_END(GameRandomArtifactConstant)

H2_ENUM_BEGIN(LayerScanConstant)
    LAYER_SCAN_CAPACITY = 5
H2_ENUM_END(LayerScanConstant)

H2_ENUM_BEGIN(RandomMapObjectFootprintConstant)
    CASTLE_METADATA_X_RADIUS      = 2,
    CASTLE_METADATA_TOP_OFFSET    = 3,
    CASTLE_METADATA_BOTTOM_OFFSET = 1,
    CASTLE_BOAT_X_OFFSET          = 1,
    CASTLE_BOAT_Y_OFFSET          = 2,
    MINE_METADATA_LEFT_OFFSET     = 2,
    MINE_METADATA_RIGHT_OFFSET    = 1,
    MINE_METADATA_TOP_OFFSET      = 1
H2_ENUM_END(RandomMapObjectFootprintConstant)

H2_ENUM_BEGIN(RandomMineConstant)
    ABANDONED_MINE_GUARDIAN_COUNT_MIN = 30,
    ABANDONED_MINE_GUARDIAN_COUNT_MAX = 60,
    WINDMILL_RESOURCE_AMOUNT_MIN      = 1,
    WINDMILL_RESOURCE_AMOUNT_MAX      = 5
H2_ENUM_END(RandomMineConstant)

inline MineType RandomMineType(MineType first, MineType last) {
    return static_cast<MineType>(Random(IDX(first), IDX(last)));
}

H2_ENUM_BEGIN(ArtifactGuardianConstant)
    ARTIFACT_GUARDIAN_CHOICE_COUNT = 10,
    MINOR_GUARDIAN_CHOICE_FIRST    = 0,
    MINOR_GUARDIAN_CHOICE_COUNT    = 4,
    MAJOR_GUARDIAN_CHOICE_FIRST    = 4,
    MAJOR_GUARDIAN_CHOICE_COUNT    = 6
H2_ENUM_END(ArtifactGuardianConstant)

H2_ENUM_BEGIN(ArtifactEventGenerationConstant)
    ARTIFACT_EVENT_UNCONDITIONAL_CUTOFF = 60,
    ARTIFACT_EVENT_GUARD_CUTOFF         = 80,
    ARTIFACT_EVENT_WISDOM_BUCKET        = 1,
    ARTIFACT_EVENT_LEADERSHIP_BUCKET    = 2
H2_ENUM_END(ArtifactEventGenerationConstant)

H2_ENUM_BEGIN(WitchHutConstant)
    WITCH_HUT_SKILL_FIRST = IDX(HERO_SKILL_PATHFINDING),
    WITCH_HUT_SKILL_LAST  = IDX(HERO_SKILL_ESTATES)
H2_ENUM_END(WitchHutConstant)

H2_ENUM_BEGIN(SkeletonEventConstant)
    SKELETON_DESERT_FRAME      = 84,
    SKELETON_ROLL_MIN          = 0,
    SKELETON_ROLL_MAX          = 9,
    SKELETON_ARTIFACT_ROLL_MAX = 2
H2_ENUM_END(SkeletonEventConstant)

H2_ENUM_BEGIN(EventGenerationRollConstant)
    EVENT_ROLL_MIN        = 0,
    EVENT_BINARY_ROLL_MAX = 1,
    EVENT_ROLL_MAX        = 100,
    EVENT_BUCKET_ROLL_MAX = 99,
    EVENT_BUCKET_COUNT    = 10
H2_ENUM_END(EventGenerationRollConstant)

H2_ENUM_BEGIN(PackedResourceGenerationConstant)
    WAGON_EMPTY_CUTOFF    = 40,
    WAGON_ARTIFACT_CUTOFF = 50,
    WAGON_AMOUNT_MIN      = 2,
    WAGON_AMOUNT_MAX      = 5,
    LEAN_TO_AMOUNT_MIN    = 1,
    LEAN_TO_AMOUNT_MAX    = 4,
    CAMPFIRE_AMOUNT_MIN   = 4,
    CAMPFIRE_AMOUNT_MAX   = 6
H2_ENUM_END(PackedResourceGenerationConstant)

H2_ENUM_BEGIN(TreasureChestGenerationConstant)
    SEA_CHEST_EMPTY_CUTOFF   = 20,
    SEA_CHEST_GOLD_CUTOFF    = 90,
    LAND_CHEST_SMALL_CUTOFF  = 32,
    LAND_CHEST_MEDIUM_CUTOFF = 64,
    LAND_CHEST_LARGE_CUTOFF  = 95
H2_ENUM_END(TreasureChestGenerationConstant)

H2_ENUM_BEGIN(AncientLampGenerationConstant)
    ANCIENT_LAMP_ROLL_MIN     = 0,
    ANCIENT_LAMP_ROLL_MAX     = 2,
    ANCIENT_LAMP_COUNT_OFFSET = 2
H2_ENUM_END(AncientLampGenerationConstant)

H2_ENUM_BEGIN(ShipwreckSurvivorGenerationConstant)
    SHIPWRECK_SURVIVOR_TREASURE_CUTOFF = 60,
    SHIPWRECK_SURVIVOR_MINOR_CUTOFF    = 80
H2_ENUM_END(ShipwreckSurvivorGenerationConstant)

H2_ENUM_BEGIN(MonsterGuardGenerationConstant)
    MONSTER_GUARD_ROLL_MIN = 0,
    MONSTER_GUARD_ROLL_MAX = 100,
    MONSTER_GUARD_CUTOFF   = 20
H2_ENUM_END(MonsterGuardGenerationConstant)

H2_ENUM_BEGIN(ResourceGenerationConstant)
    RESOURCE_BULK_AMOUNT_MIN   = 5,
    RESOURCE_BULK_AMOUNT_MAX   = 10,
    RESOURCE_SCARCE_AMOUNT_MIN = 3,
    RESOURCE_SCARCE_AMOUNT_MAX = 6
H2_ENUM_END(ResourceGenerationConstant)

H2_ENUM_BEGIN(GameVisibilityConstant)
    EARLY_TURN_LAST        = 20,
    MIDDLE_TURN_LAST       = 40,
    EARLY_AI_BONUS         = 1,
    MIDDLE_AI_BONUS        = 2,
    LATE_AI_BONUS          = 3,
    SMALL_RADIUS_LIMIT     = 5,
    SMALL_RADIUS_THRESHOLD = 2,
    LARGE_RADIUS_THRESHOLD = 3,
    RADIAL_RADIUS_LIMIT    = 10
H2_ENUM_END(GameVisibilityConstant)

H2_ENUM_BEGIN(GameLuckConstant)
    NEUTRAL       = 0,
    MINIMUM       = -3,
    MAXIMUM       = 3,
    RAINBOW_BONUS = 2
H2_ENUM_END(GameLuckConstant)

H2_ENUM_BEGIN(GameTimeEventConstant)
    EVENT_DAYS_PER_WEEK    = 7,
    EVENT_DAYS_PER_MONTH   = 28,
    EVENT_RESOURCE_COUNT   = 7,
    EVENT_RESOURCE_PENALTY = 100000
H2_ENUM_END(GameTimeEventConstant)

H2_ENUM_BEGIN(GameCompressionTestConstant)
    TEST_RANDOM_SIZE_MIN     = 20000,
    TEST_RANDOM_SIZE_MAX     = 100000,
    TEST_RANDOM_BUFFER_EXTRA = 5000,
    TEST_FILE_BUFFER_EXTRA   = 2000,
    TEST_FILENAME_SIZE       = 32,
    TEST_MESSAGE_CAPACITY    = 40,
    COMPRESS_TEST_ITERATIONS = 100
H2_ENUM_END(GameCompressionTestConstant)

H2_ENUM_BEGIN(GameRumourConstant)
    RUMOUR_SCRATCH_CAPACITY        = 100,
    RUMOUR_CATEGORY_ORDER_CAPACITY = 8
H2_ENUM_END(GameRumourConstant)

DATA(0x0052499c) b32 gbGameOver = false;

H2_ENUM_BEGIN(GameTuningConstant)
    RANDOM_SCAN_RETRY_LIMIT          = 10000,
    EXPERIENCE_HERO_PRESENCE_BONUS   = 500,
    MINE_FLAG_OVERWRITE_LIMIT        = 0x30,
    RANDOM_MONSTER_SPRITE_FIRST      = 0x43,
    RANDOM_MONSTER_SPRITE_LAST       = 0x46,
    RANDOM_MONSTER_SPRITE_TO_TRIGGER =
        0x70,
    BANK_GUARDIAN_FLAG               = 0x100,
    TOWN_RECORD_TYPE_MASK            = 0x7f
H2_ENUM_END(GameTuningConstant)

H2_ENUM_BEGIN(GamePasswordConstant)
    PASSWORD_INDEX_COUNT = X_GLOBAL_PASSWORD_STRING_INDEX_COUNT,
    PASSWORD_INDEX_MASK  = PASSWORD_INDEX_COUNT - 1,
    PASSWORD_COLOR_SHIFT = 3
H2_ENUM_END(GamePasswordConstant)

H2_ENUM_BEGIN(GameLoadMapConstant)
    LOAD_MAP_COORDINATE_SCRATCH_SIZE = 4,
    LOAD_MAP_RECORD_SCRATCH_SIZE     = 8
H2_ENUM_END(GameLoadMapConstant)

H2_ENUM_BEGIN(GameOwnershipConstant)
    TOWN_FLAG_FRAME_STRIDE       = 2,
    TOWN_FLAG_RIGHT_FRAME        = 1,
    TOWN_NEW_OWNER_TURN_COUNT    = 2,
    MINE_FLAG_COMMON_OFFSET      = 14,
    MINE_FLAG_MERCURY_OFFSET     = 21,
    MINE_FLAG_WOOD_OFFSET        = 28,
    MINE_FLAG_ALCHEMIST_OFFSET   = 35,
    MINE_FLAG_LIGHTHOUSE_OFFSET  = 42,
    MINE_ALCHEMIST_FLAG_Y_OFFSET = 3
H2_ENUM_END(GameOwnershipConstant)

H2_ENUM_BEGIN(GameViewSpellsConstant)
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
H2_ENUM_END(GameViewSpellsConstant)

H2_ENUM_BEGIN(GameViewArmyConstant)
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
        IDX(BUILDING_SLOT_UPGRADE_FIRST) - IDX(BUILDING_SLOT_DWELLING_SECOND)
H2_ENUM_END(GameViewArmyConstant)

H2_ENUM_BEGIN(GameArmyDetailText)
    ARMY_DETAIL_ATTACK        = 0,
    ARMY_DETAIL_DEFENSE       = 1,
    ARMY_DETAIL_SHOTS_LEFT    = 2,
    ARMY_DETAIL_DAMAGE        = 3,
    ARMY_DETAIL_HIT_POINTS    = 4,
    ARMY_DETAIL_SPEED         = 5,
    ARMY_DETAIL_MORALE        = 6,
    ARMY_DETAIL_LUCK          = 7,
    ARMY_DETAIL_SHOTS_OUTSIDE = 8
H2_ENUM_END(GameArmyDetailText)

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

H2_ENUM_BEGIN(ViewArmyControlId)
    VIEW_ARMY_QUICK_VIEW_ID = 0x7800,
    VIEW_ARMY_UPGRADE_ID    = 0x7803
H2_ENUM_END(ViewArmyControlId)

#define WORLDMAP (&m_worldMap)

inline town* GetCastle(i32 idx) {
    return &gpGame->m_castleRecs[idx];
}

H2_ENUM_BEGIN(PlayerDataSerializationConstant)
    PLAYER_SAVE_SCRATCH_SIZE       = 52,
    PLAYER_SAVE_SCRATCH_CLEAR_SIZE = 48,
    PLAYER_SAVE_RESERVED_SIZE      = 42,
    PLAYER_SAVE_CHEATED_FLAG_SIZE  = 1
H2_ENUM_END(PlayerDataSerializationConstant)

VA(0x0044bff0, 0x23b)
void playerData::Write(i32 file) {
    char unused[PLAYER_SAVE_SCRATCH_SIZE];

    WRITE_FILE_VALUE(file, m_color);
    WRITE_FILE_VALUE(file, m_heroCount);
    WRITE_FILE_VALUE(file, m_currentHero);
    WRITE_FILE_VALUE(file, m_heroLocatorPage);
    write(file, m_heroIds, sizeof(m_heroIds));
    write(file, m_availableHeroIds, sizeof(m_availableHeroIds));
    memset(unused, 0, PLAYER_SAVE_SCRATCH_CLEAR_SIZE);
    write(file, unused, PLAYER_SAVE_RESERVED_SIZE);
    write(file, &gpGame->m_cheated, PLAYER_SAVE_CHEATED_FLAG_SIZE);
    WRITE_FILE_VALUE(file, m_cheatValue);
    WRITE_FILE_VALUE(file, m_aiDifficulty);
    WRITE_FILE_VALUE(file, m_minimumHeroCount);
    WRITE_FILE_VALUE(file, m_evilInterface);
    WRITE_FILE_VALUE(file, m_ultimateArtifactHintChance);
    WRITE_FILE_VALUE(file, m_ultimateArtifactHintX);
    WRITE_FILE_VALUE(file, m_ultimateArtifactHintY);
    WRITE_FILE_VALUE(file, m_daysLeft);
    WRITE_FILE_VALUE(file, m_townCount);
    WRITE_FILE_VALUE(file, m_currentTown);
    WRITE_FILE_VALUE(file, m_townLocatorPage);
    write(file, m_townIds, sizeof(m_townIds));
    write(file, m_resources, sizeof(m_resources));
    write(file, m_aiData.m_income, sizeof(m_aiData.m_income));
    WRITE_FILE_VALUE(file, m_barrierTents);
    WRITE_FILE_VALUE(file, m_barrierTents);
    write(file, m_unknownad, sizeof(m_unknownad));
}

VA(0x0044c22b, 0x22c)
void playerData::Read(i32 file) {
    char unused[PLAYER_SAVE_SCRATCH_SIZE];

    READ_FILE_VALUE(file, m_color);
    READ_FILE_VALUE(file, m_heroCount);
    READ_FILE_VALUE(file, m_currentHero);
    READ_FILE_VALUE(file, m_heroLocatorPage);
    read(file, m_heroIds, sizeof(m_heroIds));
    read(file, m_availableHeroIds, sizeof(m_availableHeroIds));
    read(file, unused, PLAYER_SAVE_RESERVED_SIZE);
    read(file, &gpGame->m_cheated, PLAYER_SAVE_CHEATED_FLAG_SIZE);
    READ_FILE_VALUE(file, m_cheatValue);
    READ_FILE_VALUE(file, m_aiDifficulty);
    READ_FILE_VALUE(file, m_minimumHeroCount);
    READ_FILE_VALUE(file, m_evilInterface);
    READ_FILE_VALUE(file, m_ultimateArtifactHintChance);
    READ_FILE_VALUE(file, m_ultimateArtifactHintX);
    READ_FILE_VALUE(file, m_ultimateArtifactHintY);
    READ_FILE_VALUE(file, m_daysLeft);
    READ_FILE_VALUE(file, m_townCount);
    READ_FILE_VALUE(file, m_currentTown);
    READ_FILE_VALUE(file, m_townLocatorPage);
    read(file, m_townIds, sizeof(m_townIds));
    read(file, m_resources, sizeof(m_resources));
    read(file, m_aiData.m_income, sizeof(m_aiData.m_income));
    READ_FILE_VALUE(file, m_barrierTents);
    READ_FILE_VALUE(file, m_barrierTents);
    read(file, m_unknownad, sizeof(m_unknownad));
}

VA(0x0044c457, 0xff)
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

VA(0x0044c556, 0x50)
i32 playerData::HasMobileHero(void) {
    i32 i;
    for (i = 0; i < m_heroCount; i++) {
        if (gpGame->IsMobile(m_heroIds[i]))
            return 1;
    }
    return 0;
}

#if H2_RETAIL_COMPILER
#define index idx
#endif
VA(0x0044c5a6, 0x5a)
i32 GetNumObelisks(i32 color) {
    i32 count = 0;
    i32 index;
    for (index = 0; index < GAME_BOAT_COUNT; index++) {
        if (gpGame->m_obeliskVisitors[index] & (1 << color))
            count++;
    }
    return count;
}
#if H2_RETAIL_COMPILER
#undef index
#endif

VA(0x0044c600, 0xb4)
i32 playerData::BuildingsOwned(FactionType townType, BuildingSlotType buildingIndex, i32 buildState) {
    i32 count = 0;
    i32 i;
    for (i = 0; i < m_townCount; i++) {
        town* ownedTown = &gpGame->m_castleRecs[m_townIds[i]];
        if (buildingIndex < BUILDING_SLOT_DWELLING_FIRST || ownedTown->m_type == townType) {
            if (buildingIndex == BUILDING_SLOT_MAGE_GUILD) {
                if (HAS(ownedTown->m_buildings, IDX(TOWN_BUILDING_MAGE_GUILD))) {
                    if (ownedTown->m_buildState == buildState)
                        count++;
                }
            } else {
                if (HAS(ownedTown->m_buildings, (1 << IDX(buildingIndex))))
                    count++;
            }
        }
    }
    return count;
}

#if H2_RETAIL_COMPILER
#define artifactSlot jj
#endif
VA(0x0044c6b4, 0x84)
i32 playerData::NumOfGivenArtifact(ArtifactType artifact) {
    i32 count = 0;
    i32 i;
    i32 artifactSlot;
    for (i = 0; i < m_heroCount; i++) {
        for (artifactSlot = 0; artifactSlot < HERO_ARTIFACT_SLOT_COUNT; artifactSlot++) {
            if (gpGame->m_heroRecs[m_heroIds[i]].m_artifacts[artifactSlot] == artifact)
                count++;
        }
    }
    return count;
}
#if H2_RETAIL_COMPILER
#undef artifactSlot
#endif

#if H2_RETAIL_COMPILER
#define count num
#define mineIndex n
#endif
VA(0x0044c738, 0x6b)
i32 game::MineTypesOwned(i32 owner, MineType mineType) {
    i32 count = 0;
    i32 mineIndex;
    for (mineIndex = 0; mineIndex < GAME_MINE_COUNT; mineIndex++) {
        if (m_mines[mineIndex].owner == owner && m_mines[mineIndex].resourceType == mineType)
            count++;
    }
    return count;
}
#if H2_RETAIL_COMPILER
#undef count
#undef mineIndex
#endif

H2_ENUM_BEGIN(UltimateArtifactHintConstant)
    MINIMUM_PUZZLE_PIECES     = 8,
    HINT_CHANCE_PER_PIECE     = 4,
    HINT_CHANCE_MINIMUM       = 1,
    HINT_CHANCE_MAXIMUM       = 100,
    HINT_COORDINATE_UNKNOWN   = -1,
    HINT_OFFSET_CENTER        = 3,
    HINT_OFFSET_ROLL_MAXIMUM  = 2,
    HINT_LOCATION_RETRY_LIMIT = 200
H2_ENUM_END(UltimateArtifactHintConstant)

VA(0x0044c7a3, 0x37b)
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
                  && CELL_TERRAIN(gpGame->m_worldMap.GetCell(x, y)) != TERRAIN_WATER)
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

H2_ENUM_BEGIN(PuzzleSetupConstant)
    PUZZLE_INTERPOLATION_TERM_COUNT = 2,
    PUZZLE_INTERFACE_SEED_STRIDE    = 3,
    PUZZLE_RANDOM_STEP_MAXIMUM      = 5,
    PUZZLE_FALLBACK_RETRY_LIMIT     = 100
H2_ENUM_END(PuzzleSetupConstant)

VA(0x0044cb1e, 0x21d)
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
            if (!BitTest(puzzlePiecesRemoved, targetPiece))
                break;
        }

        for (tries = 0; tries < PUZZLE_FALLBACK_RETRY_LIMIT; tries++) {
            fallbackNum = SRandom(0, PUZZLE_PIECE_COUNT - 1);
            if (!BitTest(puzzlePiecesRemoved, fallbackNum))
                break;
        }
        if (tries >= PUZZLE_FALLBACK_RETRY_LIMIT) {
            for (fallbackNum = 0; fallbackNum < PUZZLE_PIECE_COUNT; fallbackNum++) {
                if (!BitTest(puzzlePiecesRemoved, fallbackNum))
                    break;
            }
        }
        if (fallbackNum >= PUZZLE_PIECE_COUNT)
            fallbackNum = 0;
        if (targetPiece < PUZZLE_PIECE_COUNT)
            BitSet(puzzlePiecesRemoved, targetPiece);
        else
            BitSet(puzzlePiecesRemoved, fallbackNum);
    }
    return pieceCount;
}

VA(0x0044cd3b, 0x95)
i32 game::IsMobile(i32 heroId) {
    if (heroId == -1)
        return 0;
    hero* mobileHero = &m_heroRecs[heroId];
    mapCell* cell = gpAdvManager->GetCell(mobileHero->m_x, mobileHero->m_y);
    return mobileHero->m_remainingMobility >= CalcTerrainCost(
               CELL_TERRAIN(cell),
               1,
               mobileHero->m_remainingMobility,
               IDX(mobileHero->m_secondarySkills[IDX(HERO_SKILL_PATHFINDING)]),
               cell->m_isRoad,
               0
           );
}

VA(0x0044cdd0, 0x13)
fullMap* game::GetWorldMapData(void) {
    return &m_worldMap;
}

#if H2_RETAIL_COMPILER
#define boatIndex boatIdx
#endif
VA(0x0044cde3, 0x119)
i32 game::CreateBoat(i32 x, i32 y, i32 notify) {
    i32 boatIndex = Scan(m_boatSlots, 0, GAME_BOAT_COUNT);
    if (boatIndex != -1) {
        if (notify == 0)
            SendMapChange(MAP_CHANGE_BUILD_BOAT, 0, x, y, MAP_CHANGE_CURRENT_PLAYER, 0, 0);
        m_boatSlots[boatIndex] = static_cast<i8>(boatIndex);
        boatRecord* boat = &m_boats[boatIndex];
        boat->id = static_cast<i8>(boatIndex);
        boat->x = static_cast<i8>(x);
        boat->y = static_cast<i8>(y);
        boat->direction = MAP_DIRECTION_EAST;
        boat->owner = static_cast<i8>(giCurPlayer);
        mapCell* square = WORLDMAP->GetCell(x, y);
        boat->savedTriggerType = square->m_triggerType;
        boat->savedEventData = static_cast<u8>(square->m_objectMetadata);
        square->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_BOAT);
        square->m_objectMetadata = boatIndex;
    }
    return boatIndex;
}
#if H2_RETAIL_COMPILER
#undef boatIndex
#endif

VA(0x0044cefc, 0x43)
i32 game::Scan(i8* array, i32 start, i32 length) {
    i32 i;
    for (i = start; i < start + length; i++) {
        if (array[i] == -1)
            return i;
    }
    return -1;
}

#if H2_RETAIL_COMPILER
#define index idx
#endif
VA(0x0044cf3f, 0x61)
i32 game::RandomScan(i8* array, i32 start, i32 range, i32 H2_UNUSED(unused), i8 target) {
    i32 index = target;
    i32 i;
    for (i = 0; i < RANDOM_SCAN_RETRY_LIMIT; i++) {
        index = start + Random(0, range - 1);
        if (array[index] == target)
            return index;
    }
    return -1;
}
#if H2_RETAIL_COMPILER
#undef index
#endif

#if H2_RETAIL_COMPILER
#define heroIndex heroIdx
#define unused r
#endif
VA(0x0044cfa0, 0x1a1)
i32 game::GetNewHeroId(i32, FactionType heroClass, i32 requireExperienced) {
    i32 H2_UNUSED(unused) = -1;
    i32 H2_UNUSED(previousHero);
    i32 heroIndex = -1;
    i32 attempts = 0;
    i32 H2_UNUSED(oldHeroId);
    while (attempts < HERO_SELECTION_RETRY_LIMIT) {
        attempts++;
        heroIndex = Random(0, IDX(GAME_HERO_COUNT) - 1);
        if (m_availableHeroes[heroIndex] != HERO_AVAILABILITY_UNAVAILABLE
            && m_availableHeroes[heroIndex] != WEEKLY_AVAILABLE_HERO)
            continue;
        if (m_availableHeroes[heroIndex] == WEEKLY_AVAILABLE_HERO
            && attempts < HERO_SELECTION_REUSE_RETRY_LIMIT)
            continue;
        if (heroClass >= FACTION_KNIGHT && heroClass <= FACTION_NECROMANCER
            && attempts < HERO_SELECTION_FACTION_RETRY_LIMIT
            && m_heroRecs[heroIndex].m_cursorType != heroClass)
            continue;
        if (requireExperienced && attempts < HERO_SELECTION_EXPERIENCE_RETRY_LIMIT
            && m_heroRecs[heroIndex].m_experience < HERO_SELECTION_MINIMUM_EXPERIENCE
            && (m_heroRecs[heroIndex].m_artifacts[0] == ARTIFACT_NONE
                || m_heroRecs[heroIndex].m_artifacts[0] == ARTIFACT_MAGIC_BOOK)
            && (m_heroRecs[heroIndex].m_artifacts[1] == ARTIFACT_NONE
                || m_heroRecs[heroIndex].m_artifacts[1] == ARTIFACT_MAGIC_BOOK))
            continue;
        if (gbInCampaign && attempts < HERO_SELECTION_CAMPAIGN_RETRY_LIMIT
            && m_heroRecs[heroIndex].m_portrait >= CAMPAIGN_HERO_ROLAND
            && m_heroRecs[heroIndex].m_portrait <= CAMPAIGN_HERO_BRAX)
            continue;
        break;
    }
    return heroIndex;
}
#if H2_RETAIL_COMPILER
#undef heroIndex
#undef unused
#endif

#if H2_RETAIL_COMPILER
#define column col
#endif
VA(0x0044d141, 0x5f)
i32 game::GetTownId(i32 column, i32 row) {
    i32 i;
    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        if (m_castleRecs[i].m_x == column && m_castleRecs[i].m_y == row)
            return i;
    }
    return -1;
}
#if H2_RETAIL_COMPILER
#undef column
#endif

#if H2_RETAIL_COMPILER
#define column col
#endif
VA(0x0044d1a0, 0x62)
i32 game::GetMineId(i32 column, i32 row) {
    i32 i;
    for (i = 0; i < GAME_MINE_COUNT; i++) {
        if (m_mines[i].x == column && m_mines[i].y == row)
            return i;
    }
    return -1;
}
#if H2_RETAIL_COMPILER
#undef column
#endif

#if H2_RETAIL_COMPILER
#define character chr
#define extension ext
#endif
VA(0x0044d202, 0x1ac)
void GenerateStandardFileName(char* source, char* destination) {
    char* extension = FindLastToken(source, '.');
    if (extension == NULL) {
        strcpy(destination, source);
        return;
    }

    *extension = '\0';
    i32 indexOut = 0;
    i32 length = strlen(source);
    i32 i;
    u8 character;
    for (i = 0; i < length; i++) {
        character = source[i];
        if (character >= 'a' && character <= 'z')
            character = character - ('a' - 'A');
        else if (character >= CYRILLIC_SMALL_A && character <= CYRILLIC_SMALL_YA)
            character = character - (CYRILLIC_SMALL_A - CYRILLIC_CAPITAL_A);
        else if (character == CYRILLIC_SMALL_YO)
            character = CYRILLIC_CAPITAL_YO;
        else
            character = character;
        if ((character >= 'A' && character <= 'Z') || (character >= CYRILLIC_CAPITAL_A && character <= CYRILLIC_CAPITAL_YA)
            || character == CYRILLIC_CAPITAL_YO || (character >= '0' && character <= '9') || character == '_') {
            destination[indexOut] = character;
            indexOut++;
        }
        if (indexOut >= STANDARD_FILENAME_BASENAME_SIZE)
            i = 999;
    }
    *extension = '.';
    strcpy(destination + indexOut, extension);
}
#if H2_RETAIL_COMPILER
#undef character
#undef extension
#endif

#if H2_RETAIL_COMPILER
#define playerBuffer plBuf
#define workBuffer workBuf
#endif
VA(0x0044d3ae, 0xb5a)
i32 game::SaveGame(H2_CONST char* filename, i32 generateName, i8 expansionFormat) {
    i32 nHuman;
    i32 H2_UNUSED(saveFlag);
    char workBuffer[SAVE_LEGACY_SCRATCH_SIZE];
    i32 H2_UNUSED(scratchVals)[SAVE_SPARE_SLOT_COUNT];
    char savePath[SAVE_PATH_CAPACITY];
    i32 outFile;
    i32 iFile;
    char genName[SAVE_PATH_CAPACITY];
    bchar humans[SAVE_PLAYER_FLAGS_SCRATCH_SIZE];
    char playerBuffer[SAVE_CURRENT_PLAYER_SCRATCH_SIZE];
    void* emptyPayload;
    i32 H2_UNUSED(lastTag);
    i32 chunkTag;
    i32 oldTag;

    emptyPayload = H2_ALLOC(GAME_SAVE_BUFFER_SIZE);
    memset(emptyPayload, 0, GAME_SAVE_BUFFER_SIZE);
    if (!xIsExpansionMap)
        expansionFormat = 1;
    gpAdvManager->DemobilizeCurrHero();

    if (generateName) {
        if (gbInCampaign) {
            sprintf(
                genName,
                "%s.%s",
                filename,
                "GMC"
            );
        } else if (xIsPlayingExpansionCampaign) {
            sprintf(
                genName,
                "%s.%s",
                filename,
                "GXC"
            );
        } else {
            nHuman = 0;
            for (iFile = 0; iFile < GAME_PLAYER_COUNT; iFile++) {
                if (m_playerDead[iFile] == 0 && gbHumanPlayer[iFile])
                    nHuman++;
            }
            if (xIsExpansionMap && !expansionFormat)
                sprintf(genName, "%s.GX%d", filename, nHuman);
            else
                sprintf(genName, "%s.GM%d", filename, nHuman);
        }
    } else {
        sprintf(genName, filename);
    }

    if (strnicmp(genName, "RMT", sizeof("RMT") - 1) == 0) {
        sprintf(savePath, "%s%s", ".\\DATA\\", genName);
    } else {
        sprintf(savePath, "%s%s", gcGamePath, genName);
        if (strnicmp(
                genName,
                localization::Tr("save.filename.autosave"),
                sizeof("AUTOSAVE") - 1
            )
                != 0
            && strnicmp(
                   genName,
                   localization::Tr("save.filename.player_exit"),
                   sizeof("PLYREXIT") - 1
               )
                != 0)
            strcpy(gpGame->m_saveName, filename);
    }

    outFile = open(savePath, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (outFile == -1)
        FileError(savePath);

    oldTag = -1;
    if (!expansionFormat)
        WRITE_FILE_VALUE(outFile, oldTag);
    WRITE_FILE_VALUE(outFile, m_worldMap.width);
    WRITE_FILE_VALUE(outFile, m_worldMap.height);
    WRITE_FILE_VALUE(outFile, m_mapHeader);
    write(outFile, m_setupPlayerColor, CAMPAIGN_SETUP_RESET_SIZE);
    write(outFile, &gbIAmGreatest, SAVE_TRUNCATED_SCALAR_SIZE);
    write(outFile, this, sizeof(m_difficultyRating));
    write(outFile, &giMonthType, SAVE_TRUNCATED_SCALAR_SIZE);
    write(outFile, &giMonthTypeExtra, SAVE_TRUNCATED_SCALAR_SIZE);
    write(outFile, &giWeekType, SAVE_TRUNCATED_SCALAR_SIZE);
    write(outFile, &giWeekTypeExtra, SAVE_TRUNCATED_SCALAR_SIZE);
    write(outFile, cPlayerNames, sizeof(cPlayerNames));

    memset(workBuffer, 0, SAVE_LEGACY_CLEAR_SIZE);
    write(outFile, workBuffer, SAVE_LEGACY_SERIALIZED_SIZE);
    if (xIsPlayingExpansionCampaign) {
        i32 campaignTypeInfo = SAVE_EXPANSION_CAMPAIGN_FORMAT_TAG;
        WRITE_FILE_VALUE(outFile, campaignTypeInfo);
        write(outFile, &xCampaign, CAMPAIGN_SAVE_PREFIX_SIZE);
    } else {
        WRITE_FILE_VALUE(outFile, gbInCampaign);
        if (gbInCampaign)
            write(outFile, &m_campaignType, CAMPAIGN_STATE_RESET_SIZE);
    }
    if (!expansionFormat)
        WRITE_FILE_VALUE(outFile, xIsExpansionMap);

    gpAdvManager->PurgeMapChangeQueue();
    WRITE_FILE_VALUE(outFile, giMapChangeCtr);
    GenerateStandardFileName(m_saveName, workBuffer);
    write(outFile, workBuffer, SAVE_STANDARD_FILENAME_SIZE);
    WRITE_FILE_VALUE(outFile, m_playerCount);
    playerBuffer[0] = static_cast<char>(giCurPlayer);
    write(outFile, playerBuffer, sizeof(playerBuffer[0]));
    WRITE_FILE_VALUE(outFile, m_deadPlayerCount);
    write(outFile, m_playerDead, sizeof(m_playerDead));

    for (iFile = 0; iFile < GAME_PLAYER_COUNT; iFile++) {
        humans[iFile] = static_cast<char>(gbHumanPlayer[iFile]);
        if (m_playerDead[iFile] != 0)
            humans[iFile] = false;
    }
    write(outFile, humans, GAME_PLAYER_COUNT);
    WRITE_FILE_VALUE(outFile, m_day);
    WRITE_FILE_VALUE(outFile, m_week);
    WRITE_FILE_VALUE(outFile, m_month);
    for (iFile = 0; iFile < GAME_PLAYER_COUNT; iFile++)
        m_players[iFile].Write(outFile);

    WRITE_FILE_VALUE(outFile, m_obeliskCount);
    for (iFile = 0; iFile < GAME_HERO_COUNT; iFile++)
        m_heroRecs[iFile].Write(outFile, !expansionFormat);
    write(outFile, m_availableHeroes, sizeof(m_availableHeroes));
    write(outFile, m_castleRecs, sizeof(m_castleRecs));
    write(outFile, m_castleOwners, sizeof(m_castleOwners));
    write(outFile, m_dailyEventFlags, sizeof(m_dailyEventFlags));
    write(outFile, m_mines, sizeof(m_mines));
    write(outFile, m_mineOwners, sizeof(m_mineOwners));
    if (!expansionFormat)
        write(outFile, m_randomArtifacts, IDX(ARTIFACT_COUNT));
    else
        write(outFile, m_randomArtifacts, ARTIFACT_BASE_TABLE_SIZE);
    write(outFile, m_boats, sizeof(m_boats));
    write(outFile, m_boatSlots, sizeof(m_boatSlots));
    write(outFile, m_obeliskVisitors, sizeof(m_obeliskVisitors));
    WRITE_FILE_VALUE(outFile, m_ultimateArtifactX);
    WRITE_FILE_VALUE(outFile, m_ultimateArtifactY);
    WRITE_FILE_VALUE(outFile, m_ultimateArtifactId);
    write(outFile, m_rumour, sizeof(m_rumour));
    write(outFile, m_defaultPlayerNames, sizeof(m_defaultPlayerNames));
    write(outFile, &m_rumourEventCount, SAVE_EVENT_HEADER_SIZE);
    write(
        outFile,
        m_rumourEventIndices,
        m_rumourEventCount * sizeof(m_rumourEventIndices[0])
    );
    write(outFile, &m_timeEventCount, SAVE_EVENT_HEADER_SIZE);
    write(outFile, m_timeEventIndices, m_timeEventCount * sizeof(m_timeEventIndices[0]));
    write(outFile, &m_mapEventCount, SAVE_EVENT_HEADER_SIZE);
    write(outFile, m_mapEventIndices, m_mapEventCount * sizeof(m_mapEventIndices[0]));

    chunkTag = GAME_FILE_MARKER;
    lastTag = GAME_UNUSED_FILE_MARKER;
    WRITE_FILE_VALUE(outFile, chunkTag);
    WRITE_FILE_VALUE(outFile, iMaxMapExtra);
    WRITE_FILE_VALUE(outFile, chunkTag);
    for (iFile = 1; iFile < iMaxMapExtra; iFile++) {
        WRITE_FILE_VALUE(outFile, chunkTag);
        write(outFile, pwSizeOfMapExtra + iFile, sizeof(pwSizeOfMapExtra[iFile]));
        if (ppMapExtra[iFile] != NULL)
            write(outFile, ppMapExtra[iFile], pwSizeOfMapExtra[iFile]);
        else
            write(outFile, emptyPayload, pwSizeOfMapExtra[iFile]);
    }
    WRITE_FILE_VALUE(outFile, chunkTag);
    write(outFile, mapExtra, MAP_WIDTH * MAP_HEIGHT);
    WRITE_FILE_VALUE(outFile, chunkTag);
    m_worldMap.Write(outFile);
    WRITE_FILE_VALUE(outFile, chunkTag);
    close(outFile);
    H2_FREE(emptyPayload);
    return 1;
}
#if H2_RETAIL_COMPILER
#undef playerBuffer
#undef workBuffer
#endif

VA(0x0044df08, 0x9d1)
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
    strcpy(m_saveName, localization::Tr("save.filename.new_game"));
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
                gbThisNetHumanPlayer[i] = true;
            else
                gbThisNetHumanPlayer[i] = false;
            gbHumanPlayer[i] = true;
        } else {
            gbThisNetHumanPlayer[i] = false;
            gbHumanPlayer[i] = false;
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
    gpAdvManager->m_heroContextLocked = false;
    memset(m_availableHeroes, HERO_AVAILABILITY_UNAVAILABLE, sizeof(m_availableHeroes));
    for (i = 0; i < GAME_HERO_COUNT; i++) {
        memset(&m_heroRecs[i], 0, sizeof(m_heroRecs[i]));
        memset(m_heroRecs[i].m_spells, 0, sizeof(m_heroRecs[i].m_spells));
        memset(m_heroRecs[i].m_artifacts, IDX(ARTIFACT_NONE), sizeof(m_heroRecs[i].m_artifacts));
        m_heroRecs[i].m_patrolY = HERO_DESTINATION_NONE;
        m_heroRecs[i].m_patrolX = HERO_DESTINATION_NONE;
        m_heroRecs[i].m_id = static_cast<i8>(i);
        m_heroRecs[i].m_portrait = static_cast<u8>(i);
        m_heroRecs[i].m_owner = HERO_OWNER_NONE;
        m_heroRecs[i].m_direction = MAP_DIRECTION_EAST;
        strcpy(m_heroRecs[i].m_name, gHeroDefaultNames[i]);
        m_heroRecs[i].m_cursorType = static_cast<FactionType>(i / INITIAL_RECORD_TYPE_STRIDE);
        for (j = 0; j < HERO_STARTING_STAT_COUNT; j++)
            m_heroRecs[i].m_primaryStats[j] =
                gStartingHeroStats[IDX(m_heroRecs[i].m_cursorType)][j];
        for (j = 0; j < ARMY_GROUP_SLOT_COUNT; j++)
            m_heroRecs[i].m_army.m_creatureTypes[j] = CREATURE_NONE;
        m_heroRecs[i].m_destinationY = HERO_DESTINATION_NONE;
        m_heroRecs[i].m_destinationX = HERO_DESTINATION_NONE;
        m_heroRecs[i].m_level = HERO_INITIAL_LEVEL;
        m_heroRecs[i].m_spellPoints = HERO_NORMAL_SPELL_POINTS(m_heroRecs[i]);
        m_heroRecs[i].m_secondarySkillCount = 0;
        for (j = 0; j < IDX(HERO_SKILL_COUNT); j++) {
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
    strcpy(gpGame->m_saveName, localization::Tr("save.filename.new_game"));
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

#if H2_RETAIL_COMPILER
#define fileDescriptor fd
#define index ndx
#define junkBuffer junkBuf
#define pathBuffer pathBuf
#define playerBuffer plBuf
#endif
VA(0x0044e8d9, 0xa79)
void game::LoadGame(H2_CONST char* filename, i32 loadFromFile, i32) {
    char workData[SAVE_LEGACY_CLEAR_SIZE];
    i32 H2_UNUSED(oldFlag);
    char isHuman[SAVE_PLAYER_FLAGS_SCRATCH_SIZE];
    i32 H2_UNUSED(saveVal);
    i32 rows;
    char pathBuffer[SAVE_PATH_CAPACITY];
    b8 expTag;
    i32 fileDescriptor;
    char H2_UNUSED(junkBuffer)[SAVE_LEGACY_CLEAR_SIZE];
    i32 index;
    char playerBuffer[LOAD_CURRENT_PLAYER_SCRATCH_SIZE];
    char chunkTag[LOAD_CURRENT_PLAYER_SCRATCH_SIZE];
    i32 numHumans;
    i32 wide;

    LogStr("LG1");
    if (loadFromFile) {
        SetupOrigData();
        return;
    }
    LogStr("LG2");
    numHumans = 0;
    gbGameOver = false;
    m_gameLoaded = 1;

    if (loadFromFile || strnicmp(filename, "RMT", sizeof("RMT") - 1) == 0)
        sprintf(pathBuffer, "%s%s", ".\\DATA\\", filename);
    else
        sprintf(pathBuffer, "%s%s", gcGamePath, filename);

    fileDescriptor = open(pathBuffer, _O_BINARY);
    if (fileDescriptor == -1)
        FileError(pathBuffer);
    ClearMapExtra();

    expTag = false;
    READ_FILE_VALUE(fileDescriptor, wide);
    if (wide == -1) {
        expTag = true;
        READ_FILE_VALUE(fileDescriptor, wide);
    }
    READ_FILE_VALUE(fileDescriptor, rows);
    SetMapSize(wide, rows);
    READ_FILE_VALUE(fileDescriptor, m_mapHeader);
    read(fileDescriptor, m_setupPlayerColor, CAMPAIGN_SETUP_RESET_SIZE);
    read(fileDescriptor, &gbIAmGreatest, SAVE_TRUNCATED_SCALAR_SIZE);
    read(fileDescriptor, this, sizeof(m_difficultyRating));
    read(fileDescriptor, &giMonthType, SAVE_TRUNCATED_SCALAR_SIZE);
    read(fileDescriptor, &giMonthTypeExtra, SAVE_TRUNCATED_SCALAR_SIZE);
    read(fileDescriptor, &giWeekType, SAVE_TRUNCATED_SCALAR_SIZE);
    read(fileDescriptor, &giWeekTypeExtra, SAVE_TRUNCATED_SCALAR_SIZE);
    read(fileDescriptor, cPlayerNames, sizeof(cPlayerNames));

    read(fileDescriptor, workData, SAVE_LEGACY_SERIALIZED_SIZE);
    READ_FILE_VALUE(fileDescriptor, gbInCampaign);
    if (gbInCampaign == 1) {
        read(fileDescriptor, &m_campaignType, CAMPAIGN_STATE_RESET_SIZE);
    } else if (gbInCampaign == SAVE_EXPANSION_CAMPAIGN_FORMAT_TAG) {
        xIsPlayingExpansionCampaign = 1;
        gbInCampaign = false;
        read(fileDescriptor, &xCampaign, CAMPAIGN_SAVE_PREFIX_SIZE);
    }
    if (expTag)
        READ_FILE_VALUE(fileDescriptor, xIsExpansionMap);

    gpAdvManager->PurgeMapChangeQueue();
    READ_FILE_VALUE(fileDescriptor, giMapChangeCtr);
    read(fileDescriptor, workData, SAVE_STANDARD_FILENAME_SIZE);
    if (strnicmp(filename, "RMT", sizeof("RMT") - 1) != 0)
        sprintf(gpGame->m_saveName, filename);
    READ_FILE_VALUE(fileDescriptor, m_playerCount);

    read(fileDescriptor, playerBuffer, sizeof(playerBuffer[0]));
    giCurPlayer = playerBuffer[0];
    READ_FILE_VALUE(fileDescriptor, m_deadPlayerCount);
    read(fileDescriptor, m_playerDead, sizeof(m_playerDead));

    read(fileDescriptor, isHuman, GAME_PLAYER_COUNT);
    for (index = 0; index < GAME_PLAYER_COUNT; index++) {
        if (isHuman[index] && numHumans < giNumHumanPlayers) {
            numHumans++;
            gbHumanPlayer[index] = true;
        } else {
            gbHumanPlayer[index] = false;
        }
    }
    for (index = 0; index < GAME_PLAYER_COUNT; index++) {
        if (gbHumanPlayer[index]) {
            if (!gbRemoteOn || index == giThisGamePos)
                gbThisNetHumanPlayer[index] = true;
            else
                gbThisNetHumanPlayer[index] = false;
        } else {
            gbThisNetHumanPlayer[index] = false;
        }
    }

    READ_FILE_VALUE(fileDescriptor, m_day);
    READ_FILE_VALUE(fileDescriptor, m_week);
    READ_FILE_VALUE(fileDescriptor, m_month);
    giCurTurn = GAME_DAY_NUMBER(*this);
    for (index = 0; index < GAME_PLAYER_COUNT; index++)
        m_players[index].Read(fileDescriptor);

    READ_FILE_VALUE(fileDescriptor, m_obeliskCount);
    for (index = 0; index < GAME_HERO_COUNT; index++)
        m_heroRecs[index].Read(fileDescriptor, expTag);
    read(fileDescriptor, m_availableHeroes, sizeof(m_availableHeroes));
    read(fileDescriptor, m_castleRecs, sizeof(m_castleRecs));
    read(fileDescriptor, m_castleOwners, sizeof(m_castleOwners));
    read(fileDescriptor, m_dailyEventFlags, sizeof(m_dailyEventFlags));
    read(fileDescriptor, m_mines, sizeof(m_mines));
    read(fileDescriptor, m_mineOwners, sizeof(m_mineOwners));
    if (expTag)
        read(fileDescriptor, m_randomArtifacts, IDX(ARTIFACT_COUNT));
    else
        read(fileDescriptor, m_randomArtifacts, ARTIFACT_BASE_TABLE_SIZE);
    read(fileDescriptor, m_boats, sizeof(m_boats));
    read(fileDescriptor, m_boatSlots, sizeof(m_boatSlots));
    read(fileDescriptor, m_obeliskVisitors, sizeof(m_obeliskVisitors));
    READ_FILE_VALUE(fileDescriptor, m_ultimateArtifactX);
    READ_FILE_VALUE(fileDescriptor, m_ultimateArtifactY);
    READ_FILE_VALUE(fileDescriptor, m_ultimateArtifactId);
    read(fileDescriptor, m_rumour, sizeof(m_rumour));
    read(fileDescriptor, m_defaultPlayerNames, sizeof(m_defaultPlayerNames));
    read(fileDescriptor, &m_rumourEventCount, SAVE_EVENT_HEADER_SIZE);
    read(
        fileDescriptor,
        m_rumourEventIndices,
        m_rumourEventCount * sizeof(m_rumourEventIndices[0])
    );
    read(fileDescriptor, &m_timeEventCount, SAVE_EVENT_HEADER_SIZE);
    read(fileDescriptor, m_timeEventIndices, m_timeEventCount * sizeof(m_timeEventIndices[0]));
    read(fileDescriptor, &m_mapEventCount, SAVE_EVENT_HEADER_SIZE);
    read(fileDescriptor, m_mapEventIndices, m_mapEventCount * sizeof(m_mapEventIndices[0]));

    read(fileDescriptor, chunkTag, sizeof(i32));
    READ_FILE_VALUE(fileDescriptor, iMaxMapExtra);
    read(fileDescriptor, chunkTag, sizeof(i32));
    ppMapExtra = reinterpret_cast<void**>(
        H2_ALLOC(iMaxMapExtra * sizeof(*ppMapExtra))
    );
    pwSizeOfMapExtra = reinterpret_cast<i16*>(
        H2_ALLOC(iMaxMapExtra * sizeof(*pwSizeOfMapExtra))
    );
    memset(ppMapExtra, 0, iMaxMapExtra * sizeof(*ppMapExtra));
    memset(pwSizeOfMapExtra, 0, iMaxMapExtra * sizeof(*pwSizeOfMapExtra));
    for (index = 1; index < iMaxMapExtra; index++) {
        read(fileDescriptor, chunkTag, sizeof(i32));
        read(fileDescriptor, pwSizeOfMapExtra + index, sizeof(pwSizeOfMapExtra[index]));
        ppMapExtra[index] = H2_ALLOC(pwSizeOfMapExtra[index]);
        read(fileDescriptor, ppMapExtra[index], pwSizeOfMapExtra[index]);
    }
    read(fileDescriptor, chunkTag, sizeof(i32));
    read(fileDescriptor, mapExtra, MAP_WIDTH * MAP_HEIGHT);
    read(fileDescriptor, chunkTag, sizeof(i32));
    m_worldMap.Read(fileDescriptor, 0);
    read(fileDescriptor, chunkTag, sizeof(i32));
    close(fileDescriptor);

    gpAdvManager->m_heroContextLocked = false;
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
#if H2_RETAIL_COMPILER
#undef fileDescriptor
#undef index
#undef junkBuffer
#undef pathBuffer
#undef playerBuffer
#endif

VA(0x0044f352, 0x37d)
void game::GiveTroopsToNeutralTown(i32 townId) {
    i32 roll;
    CreatureType monster;
    i32 H2_UNUSED(unused);
    i32 tierBase;
    i32 turnBonus;
    i32 howMany;

    if ((m_castleRecs[townId].m_x > 0 || m_castleRecs[townId].m_y > 0)
        && m_castleRecs[townId].m_owner < 0) {
        roll = Random(REINFORCEMENT_ROLL_MIN, REINFORCEMENT_ROLL_MAX);
        turnBonus = giCurTurn / REINFORCEMENT_TURN_ROLL_DIVISOR;
        if (turnBonus != 0)
            roll += Random(0, turnBonus);

        if (roll <= REINFORCEMENT_TIER_ONE_THRESHOLD) {
            tierBase = REINFORCEMENT_TIER_ONE_KEY;
            howMany = Random(REINFORCEMENT_TIER_ONE_COUNT_MIN, REINFORCEMENT_TIER_ONE_COUNT_MAX);
        } else if (roll <= REINFORCEMENT_TIER_TWO_THRESHOLD) {
            tierBase = REINFORCEMENT_TIER_TWO_KEY;
            howMany = Random(REINFORCEMENT_TIER_TWO_COUNT_MIN, REINFORCEMENT_TIER_TWO_COUNT_MAX);
        } else if (roll <= REINFORCEMENT_TIER_THREE_THRESHOLD) {
            tierBase = REINFORCEMENT_TIER_THREE_KEY;
            howMany = Random(REINFORCEMENT_TIER_THREE_COUNT_MIN, REINFORCEMENT_TIER_THREE_COUNT_MAX);
        } else if (roll <= REINFORCEMENT_TIER_FOUR_THRESHOLD) {
            tierBase = REINFORCEMENT_TIER_FOUR_KEY;
            howMany = Random(REINFORCEMENT_TIER_FOUR_COUNT_MIN, REINFORCEMENT_TIER_FOUR_COUNT_MAX);
        } else {
            tierBase = REINFORCEMENT_TIER_FIVE_KEY;
            howMany = REINFORCEMENT_TIER_FIVE_COUNT;
        }

        howMany += giCurTurn / REINFORCEMENT_TURN_COUNT_DIVISOR;
        switch (tierBase + IDX(m_castleRecs[townId].m_type)) {
            case REINFORCEMENT_TIER_ONE_KEY + IDX(FACTION_KNIGHT):
                monster = CREATURE_PEASANT;
                break;
            case REINFORCEMENT_TIER_TWO_KEY + IDX(FACTION_KNIGHT):
                monster = CREATURE_ARCHER;
                break;
            case REINFORCEMENT_TIER_THREE_KEY + IDX(FACTION_KNIGHT):
                monster = CREATURE_PIKEMAN;
                break;
            case REINFORCEMENT_TIER_FOUR_KEY + IDX(FACTION_KNIGHT):
                monster = CREATURE_SWORDSMAN;
                break;
            case REINFORCEMENT_TIER_FIVE_KEY + IDX(FACTION_KNIGHT):
                monster = CREATURE_CAVALRY;
                break;
            case REINFORCEMENT_TIER_ONE_KEY + IDX(FACTION_BARBARIAN):
                monster = CREATURE_GOBLIN;
                break;
            case REINFORCEMENT_TIER_TWO_KEY + IDX(FACTION_BARBARIAN):
                monster = CREATURE_ORC;
                break;
            case REINFORCEMENT_TIER_THREE_KEY + IDX(FACTION_BARBARIAN):
                monster = CREATURE_WOLF;
                break;
            case REINFORCEMENT_TIER_FOUR_KEY + IDX(FACTION_BARBARIAN):
                monster = CREATURE_OGRE;
                break;
            case REINFORCEMENT_TIER_FIVE_KEY + IDX(FACTION_BARBARIAN):
                monster = CREATURE_TROLL;
                break;
            case REINFORCEMENT_TIER_ONE_KEY + IDX(FACTION_SORCERESS):
                monster = CREATURE_SPRITE;
                break;
            case REINFORCEMENT_TIER_TWO_KEY + IDX(FACTION_SORCERESS):
                monster = CREATURE_DWARF;
                break;
            case REINFORCEMENT_TIER_THREE_KEY + IDX(FACTION_SORCERESS):
                monster = CREATURE_ELF;
                break;
            case REINFORCEMENT_TIER_FOUR_KEY + IDX(FACTION_SORCERESS):
                monster = CREATURE_DRUID;
                break;
            case REINFORCEMENT_TIER_FIVE_KEY + IDX(FACTION_SORCERESS):
                monster = CREATURE_UNICORN;
                break;
            case REINFORCEMENT_TIER_ONE_KEY + IDX(FACTION_WARLOCK):
                monster = CREATURE_CENTAUR;
                break;
            case REINFORCEMENT_TIER_TWO_KEY + IDX(FACTION_WARLOCK):
                monster = CREATURE_GARGOYLE;
                break;
            case REINFORCEMENT_TIER_THREE_KEY + IDX(FACTION_WARLOCK):
                monster = CREATURE_GRIFFIN;
                break;
            case REINFORCEMENT_TIER_FOUR_KEY + IDX(FACTION_WARLOCK):
                monster = CREATURE_MINOTAUR;
                break;
            case REINFORCEMENT_TIER_FIVE_KEY + IDX(FACTION_WARLOCK):
                monster = CREATURE_HYDRA;
                break;
            case REINFORCEMENT_TIER_ONE_KEY + IDX(FACTION_WIZARD):
                monster = CREATURE_HALFLING;
                break;
            case REINFORCEMENT_TIER_TWO_KEY + IDX(FACTION_WIZARD):
                monster = CREATURE_BOAR;
                break;
            case REINFORCEMENT_TIER_THREE_KEY + IDX(FACTION_WIZARD):
                monster = CREATURE_IRON_GOLEM;
                break;
            case REINFORCEMENT_TIER_FOUR_KEY + IDX(FACTION_WIZARD):
                monster = CREATURE_ROC;
                break;
            case REINFORCEMENT_TIER_FIVE_KEY + IDX(FACTION_WIZARD):
                monster = CREATURE_MAGE;
                break;
            case REINFORCEMENT_TIER_ONE_KEY + IDX(FACTION_NECROMANCER):
                monster = CREATURE_SKELETON;
                break;
            case REINFORCEMENT_TIER_TWO_KEY + IDX(FACTION_NECROMANCER):
                monster = CREATURE_ZOMBIE;
                break;
            case REINFORCEMENT_TIER_THREE_KEY + IDX(FACTION_NECROMANCER):
                monster = CREATURE_MUMMY;
                break;
            case REINFORCEMENT_TIER_FOUR_KEY + IDX(FACTION_NECROMANCER):
                monster = CREATURE_VAMPIRE;
                break;
            case REINFORCEMENT_TIER_FIVE_KEY + IDX(FACTION_NECROMANCER):
                monster = CREATURE_LICH;
                break;
            default:;
        }
        GiveArmy(&m_castleRecs[townId].m_army, monster, howMany, ARMY_GROUP_EMPTY_SLOT);
    }
}

VA(0x0044f6cf, 0x86)
void game::GiveTroopsToNeutralTowns(void) {
    i32 i;
    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        GiveTroopsToNeutralTown(i);
        if (HAS(m_castleRecs[i].m_buildings, IDX(TOWN_BUILDING_CASTLE))) {
            if (Random(0, REINFORCEMENT_ROLL_PERCENT_MAX) < REINFORCEMENT_CASTLE_CHANCE)
                GiveTroopsToNeutralTown(i);
        } else {
            if (Random(0, REINFORCEMENT_ROLL_PERCENT_MAX) < REINFORCEMENT_CHANCE)
                GiveTroopsToNeutralTown(i);
        }
    }
}

#if H2_RETAIL_COMPILER
#define dotPosition dotPos
#define humanPosition humanPos
#define xPosition xPos
#define yPosition yPos
#endif
VA(0x0044f755, 0x1c7a)
void game::NewMap(char* filename) {
    FactionType sideClass;
    H2_CONST char* heroName;
    HeroPortrait curPic;
    FactionType specClass;
    i32 awardHero;
    i32 H2_UNUSED(padNum);
    i32 selectedTown;
    i32 H2_UNUSED(heroIndex);
    i32 humanPosition;
    FactionType startClass;
    i32 nTown;
    i32 player;
    i32 yPosition;
    i32 ultimateTries;
    i32 xPosition;
    FactionType race;
    char* dotPosition;
    i32 H2_UNUSED(junkVal);
    i32 iPass;
    i32 ultimateDistance;

    dotPosition = FindLastToken(gMapName, '.');
    if (dotPosition != NULL && StrEqNoCase(dotPosition + 1, "MX2"))
        xIsExpansionMap = true;
    if (xIsExpansionMap)
        gTownEligibleBuildMask[IDX(FACTION_NECROMANCER)] |= NECROMANCER_SHRINE_BUILD_MASK;
    else
        gTownEligibleBuildMask[IDX(FACTION_NECROMANCER)] &= ~NECROMANCER_SHRINE_BUILD_MASK;

    gbInNewGameSetup = true;
    giCurPlayer = 0;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);
    giCurWatchPlayerBit = giCurPlayerBit;
    giCurWatchPlayer = giCurPlayer;
    race = static_cast<FactionType>(Random(0, GAME_PLAYER_COUNT - 1));
    humanPosition = giNumHumanPlayers;

    for (player = 0; player < GAME_PLAYER_COUNT; player++) {
        if (player >= gpGame->m_mapHeader.playerCount) {
            gbSetupGamePosToRealGamePos[player] = -1;
        } else {
            if (m_setupPlayerNetworkId[player] == GAME_COMPUTER_PLAYER)
                gbSetupGamePosToRealGamePos[player] = static_cast<i8>(humanPosition++);
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
                        && m_castleRecs[(m_players + player)->m_townIds[nTown]].m_occupyingHeroId
                               == -1
                        && (HAS(m_castleRecs[(m_players + player)->m_townIds[nTown]].m_buildings,
                                IDX(TOWN_BUILDING_CASTLE))
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
                giVisRange[IDX(m_heroRecs[m_players[player].m_heroIds[0]]
                                   .m_secondarySkills[IDX(HERO_SKILL_SCOUTING)])]
            );
            m_players[player].m_heroCount++;
        }
    }

    for (player = 0; player < m_playerCount; player++) {
        if (player == 0 && gbInCampaign
            && (m_campaignAwards[IDX(CAMPAIGN_AWARD_SORCERESS_GUILD)] != 0
                || m_campaignAwards[IDX(CAMPAIGN_AWARD_NECROMANCER_GUILD)] != 0)) {
            specClass = m_campaignAwards[IDX(CAMPAIGN_AWARD_SORCERESS_GUILD)]
                                  ? FACTION_SORCERESS
                                  : FACTION_NECROMANCER;
            for (awardHero = 0; awardHero < GAME_HERO_COUNT; awardHero++) {
                if (m_heroRecs[awardHero].m_cursorType == specClass
                    && m_availableHeroes[awardHero] == -1)
                    break;
            }
            if (awardHero < GAME_HERO_COUNT) {
                if (m_campaignAwards[IDX(CAMPAIGN_AWARD_SORCERESS_GUILD)] != 0) {
                    ADD_HERO_EXPERIENCE_AND_CHECK_LEVEL(
                        m_heroRecs[awardHero],
                        CAMPAIGN_EXPERIENCE_BONUS
                    );
                    strcpy(
                        m_heroRecs[awardHero].m_name,
                        localization::Tr("campaign.hero.sister_eliza")
                    );
                    m_heroRecs[awardHero].m_portrait = CAMPAIGN_HERO_ELIZA;
                } else {
                    ADD_HERO_EXPERIENCE_AND_CHECK_LEVEL(
                        m_heroRecs[awardHero],
                        CAMPAIGN_EXPERIENCE_BONUS
                    );
                    strcpy(
                        m_heroRecs[awardHero].m_name,
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
                        strcpy(m_heroRecs[awardHero].m_name, heroName);
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
            startClass = static_cast<FactionType>(Random(0, IDX(FACTION_COUNT) - 1));
            if (m_setupPlayerRace[gcColorToSetupPos[m_players[player].m_color]]
                < FACTION_COUNT)
                startClass = m_setupPlayerRace[gcColorToSetupPos[m_players[player].m_color]];
            m_players[player].m_availableHeroIds[0] =
                static_cast<char>(GetNewHeroId(player, startClass, 0));
            m_availableHeroes[m_players[player].m_availableHeroIds[0]] = WEEKLY_AVAILABLE_HERO;
        }
    secondHero:
        startClass = (startClass + Random(1, IDX(FACTION_COUNT) - 1)) % IDX(FACTION_COUNT);
        m_players[player].m_availableHeroIds[1] =
            static_cast<char>(GetNewHeroId(player, startClass, 0));
        m_availableHeroes[m_players[player].m_availableHeroIds[1]] = WEEKLY_AVAILABLE_HERO;
    }

    for (player = 0; player < m_playerCount; player++) {
        for (nTown = 0; nTown < m_players[player].m_heroCount;
             nTown++) {
            xPosition = m_heroRecs[m_players[player].m_heroIds[nTown]].m_x;
            yPosition = m_heroRecs[m_players[player].m_heroIds[nTown]].m_y;
            m_heroRecs[m_players[player].m_heroIds[nTown]].m_locationType =
                m_worldMap.GetCell(xPosition, yPosition)->m_triggerType;
            m_heroRecs[m_players[player].m_heroIds[nTown]].m_occupiedTown =
                m_worldMap.GetCell(xPosition, yPosition)->m_objectMetadata;
            m_worldMap.GetCell(xPosition, yPosition)->m_triggerType =
                MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION);
            m_worldMap.GetCell(xPosition, yPosition)->m_objectMetadata =
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
    while (player < ULTIMATE_ARTIFACT_BORDER_MARGIN || nTown < ULTIMATE_ARTIFACT_BORDER_MARGIN
           || player > MAP_WIDTH - ULTIMATE_ARTIFACT_BORDER_MARGIN - 1
           || nTown > MAP_HEIGHT - ULTIMATE_ARTIFACT_BORDER_MARGIN - 1
           || m_worldMap.GetCell(player, nTown)->m_objectIndex != MAPCELL_SPRITE_NONE
           || m_worldMap.GetCell(player, nTown)->m_overlayIndex != MAPCELL_SPRITE_NONE
           || CELL_TERRAIN(m_worldMap.GetCell(player, nTown)) == TERRAIN_WATER
           || (giNumHumanPlayers == 1 && ultimateTries < ULTIMATE_HUMAN_DISTANCE_RETRY_LIMIT
               && ultimateDistance >= MANHATTAN_LENGTH(
                      player - m_heroRecs[m_players[0].m_heroIds[0]].m_x,
                      nTown - m_heroRecs[m_players[0].m_heroIds[0]].m_y
                  ))) {
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
        static_cast<ArtifactType>(Random(IDX(ARTIFACT_ULTIMATE_BOOK), IDX(ARTIFACT_GOLDEN_GOOSE)));
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
                gInitResourcesHuman[IDX(m_difficulty)],
                sizeof(m_players[player].m_resources)
            );
            if (m_playerHandicap[player] != PLAYER_HANDICAP_NONE) {
                for (nTown = 0; nTown < IDX(RES_COUNT); nTown++) {
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
                IDX(PLAYER_PERSONALITY_COMPUTER_FIRST),
                IDX(PLAYER_PERSONALITY_COMPUTER_LAST)
            ));
            memcpy(
                m_players[player].m_resources,
                gInitResourcesComputer[IDX(m_difficulty)],
                sizeof(m_players[player].m_resources)
            );
        }
    }
    SetupAdjacentMons();
    if (m_mapHeader.lossCondition == MAP_LOSS_HERO) {
        xPosition = m_mapHeader.lossConditionValue;
        yPosition = m_mapHeader.lossTownY;
        m_mapHeader.lossConditionValue = 0;
        if (m_worldMap.GetCell(xPosition, yPosition)->m_triggerType
            == (MAP_ACTION_TRIGGER(MAP_OBJECT_MERMAID)))
            m_mapHeader.lossConditionValue =
                m_worldMap.GetCell(xPosition, yPosition)->m_objectMetadata;
        else {
            if (m_worldMap.GetCell(xPosition, yPosition - 1)->m_triggerType
                == (MAP_ACTION_TRIGGER(MAP_OBJECT_MERMAID)))
                m_mapHeader.lossConditionValue =
                    m_worldMap.GetCell(xPosition, yPosition - 1)->m_objectMetadata;
            else
                m_mapHeader.lossCondition = MAP_LOSS_STANDARD;
        }
    }
    if (m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO) {
        xPosition = m_mapHeader.victoryConditionValue;
        yPosition = m_mapHeader.victoryTownY;
        m_mapHeader.victoryConditionValue = 0;
        if (m_worldMap.GetCell(xPosition, yPosition)->m_triggerType
            == (MAP_ACTION_TRIGGER(MAP_OBJECT_MERMAID)))
            m_mapHeader.victoryConditionValue =
                m_worldMap.GetCell(xPosition, yPosition)->m_objectMetadata;
        else {
            if (m_worldMap.GetCell(xPosition, yPosition - 1)->m_triggerType
                == (MAP_ACTION_TRIGGER(MAP_OBJECT_MERMAID)))
                m_mapHeader.victoryConditionValue =
                    m_worldMap.GetCell(xPosition, yPosition - 1)->m_objectMetadata;
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
            m_players[player].m_evilInterface = true;
        else
            m_players[player].m_evilInterface = false;
        if (gbInCampaign && player == 0) {
            if (m_campaignType == CAMPAIGN_ARCHIBALD)
                m_players[player].m_evilInterface = true;
            else
                m_players[player].m_evilInterface = false;
        }
        for (nTown = 0; nTown < gpGame->m_players[player].m_townCount; nTown++)
            GetCastle(gpGame->m_players[player].m_townIds[nTown])->GiveSpells(NULL);
        gpGame->m_players[player].m_minimumHeroCount = gpGame->m_players[player].m_heroCount;
    }
    gpPhilAI->GetGameAIVars();
    gbInNewGameSetup = false;
    SetupNewRumour();
    gpAdvManager->CheckSetEvilInterface(0, -1);
    return;
}
#if H2_RETAIL_COMPILER
#undef dotPosition
#undef humanPosition
#undef xPosition
#undef yPosition
#endif

inline town* GetCastleSlot(game* instance, i32 index) {
    return &instance->m_castleRecs[index];
}

#if H2_RETAIL_COMPILER
#define artifactGuardianChoices artifactGuardianChoices1
#define below below0
#define cell cell2
#define column column3
#define eventData eventData4
#define extra extra9
#define extraIndex extraIndex27
#define eyeId eyeId13
#define hutId hutId27
#define jailId jailId8
#define lowerIndexes lowerIndexes28
#define lowerTileIndex j4
#define lowerTilesets lowerTilesets5
#define mapEvent mapEvent0
#define mineId mineId6
#define randomValue randomValue5
#define shrineId shrineId8
#define tentId tentId0
#define townEntrance townEntrance2
#define upperCount upperCount5
#define upperIndexes upperIndexes8
#define upperTilesets upperTilesets0
#define xPosition xPos
#define yPosition yPos
#endif
VA(0x004513cf, 0x26c6)
void game::RandomizeEvents(void) {
    b32 valid;
    u32 extraIndex;
    i32 row;
    i32 xPosition;
    ArtifactType value;
    i32 shrineId = 1;
    i32 bottleId = 1;
    i32 jailId = 1;
    i32 sphinxId = 1;
    i32 tentId = 1;
    i32 hutId = 1;
    i32 eyeId = 1;
    i32 signId = 1;
    i32 lowerTileIndex;
    mapCell* cell;
    i32 yPosition;
    i32 randomValue;
    i32 column;
    mapCellExtra* extra;
    mapEventExtra* eventData;
    i32 mineId;
    EventExtra* mapEvent;
    town* townRec;
    mapCell* townEntrance;
    CreatureType artifactGuardianChoices[ARTIFACT_GUARDIAN_CHOICE_COUNT];
    i32 lowerCount;
    i32 upperCount;
    TilesetId lowerTilesets[LAYER_SCAN_CAPACITY];
    TilesetId upperTilesets[LAYER_SCAN_CAPACITY];
    i32 upperIndexes[LAYER_SCAN_CAPACITY];
    i32 lowerIndexes[LAYER_SCAN_CAPACITY];

    m_mapEventCount = 0;
    memset(m_mapEventIndices, 0, sizeof(m_mapEventIndices));

    for (yPosition = 0; yPosition < MAP_HEIGHT; yPosition++) {
        for (xPosition = 0; xPosition < MAP_WIDTH; xPosition++) {
            cell = m_worldMap.GetCell(xPosition, yPosition);
            switch (cell->m_triggerType) {
                case MAP_ACTION_TRIGGER(MAP_OBJECT_WITCH_HUT):
                    cell->m_objectMetadata = IDX(HERO_SKILL_NECROMANCY);
                    while (cell->m_objectMetadata == IDX(HERO_SKILL_NECROMANCY)
                           || cell->m_objectMetadata == IDX(HERO_SKILL_LEADERSHIP)) {
                        cell->m_objectMetadata =
                            Random(WITCH_HUT_SKILL_FIRST, WITCH_HUT_SKILL_LAST);
                    }
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_BOAT):
                    cell->m_objectTileset = TILESET_NONE;
                    cell->m_objectIndex = MAPCELL_SPRITE_NONE;
                    cell->m_objectMetadata = 0;
                    cell->m_triggerType = 0;
                    CreateBoat(xPosition, yPosition, 1);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_SPHINX):
                    eventData =
                        reinterpret_cast<mapEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
                    if (strlen(eventData->riddle) > 1 && eventData->answerCount >= 1)
                        eventData->active = 1;
                    else
                        eventData->active = 0;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_MAP_EVENT):
                    m_mapEventIndices[m_mapEventCount] = cell->m_objectMetadata;
                    mapEvent = reinterpret_cast<EventExtra*>(ppMapExtra[cell->m_objectMetadata]);
                    mapEvent->x = static_cast<i16>(xPosition);
                    mapEvent->y = static_cast<i16>(yPosition);
                    mapEvent->active = true;
                    cell->m_objectMetadata = 0;
                    cell->m_triggerType = 0;
                    cell->m_objectIndex = MAPCELL_SPRITE_NONE;
                    cell->m_objectTileset = TILESET_NONE;
                    m_mapEventCount++;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_GAZEBO):
                    cell->m_objectMetadata = bottleId++;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_FORT):
                    cell->m_objectMetadata = jailId++;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_WITCH_DOCTOR_HUT):
                    cell->m_objectMetadata = sphinxId;
                    sphinxId++;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_MERCENARY_CAMP):
                    cell->m_objectMetadata = tentId++;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_STANDING_STONES):
                    if (xPosition > 0
                        && m_worldMap.GetCell(xPosition - 1, yPosition)->m_triggerType
                               == (MAP_ACTION_TRIGGER(MAP_OBJECT_STANDING_STONES)))
                        cell->m_objectMetadata =
                            m_worldMap.GetCell(xPosition - 1, yPosition)->m_objectMetadata;
                    else
                        cell->m_objectMetadata = hutId++;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_XANADU):
                    cell->m_objectMetadata = signId++;
                    break;
                case MAP_PASSIVE_TRIGGER(MAP_OBJECT_WHIRLPOOL):
                    cell->m_triggerType |= MAP_TRIGGER_ACTION_FLAG;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_OBELISK):
                    cell->m_objectMetadata = shrineId++;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_FLOTSAM):
                    cell->m_objectMetadata =
                        Random(IDX(FLOTSAM_EMPTY), IDX(FLOTSAM_LARGE_TREASURE));
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_SKELETON):
                    if (!HasObjectTilesetIndex(
                            xPosition,
                            yPosition,
                            TILESET_OBJNDSRT,
                            SKELETON_DESERT_FRAME
                        ))
                        cell->m_triggerType &= MAP_TRIGGER_TYPE_MASK;
                    else
                        cell->m_objectMetadata = Random(SKELETON_ROLL_MIN, SKELETON_ROLL_MAX)
                                                          <= SKELETON_ARTIFACT_ROLL_MAX
                                                      ? GetRandomArtifactId(
                                                            ARTIFACT_LEVEL_RANDOM,
                                                            true
                                                        ) + SKELETON_ARTIFACT_OFFSET
                                                      : SKELETON_EMPTY;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_WAGON):
                    randomValue = Random(EVENT_ROLL_MIN, EVENT_ROLL_MAX);
                    if (randomValue < WAGON_EMPTY_CUTOFF)
                        cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                    else if (randomValue < WAGON_ARTIFACT_CUTOFF)
                        cell->m_objectMetadata =
                            GetRandomArtifactId(
                                ARTIFACT_LEVEL_MINOR | ARTIFACT_LEVEL_TREASURE,
                                true
                            )
                            | WAGON_ARTIFACT_FLAG;
                    else
                        cell->m_objectMetadata =
                            Random(IDX(RES_WOOD), IDX(RES_GEMS))
                            + (Random(WAGON_AMOUNT_MIN, WAGON_AMOUNT_MAX)
                               << CAMPFIRE_AMOUNT_SHIFT)
                            + MAP_EVENT_RESOURCE_OFFSET;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_LEAN_TO):
                    cell->m_objectMetadata =
                        Random(IDX(RES_WOOD), IDX(RES_GEMS))
                        + (Random(LEAN_TO_AMOUNT_MIN, LEAN_TO_AMOUNT_MAX)
                           << CAMPFIRE_AMOUNT_SHIFT)
                        + MAP_EVENT_RESOURCE_OFFSET;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_DAEMON_CAVE):
                    switch (Random(EVENT_ROLL_MIN, EVENT_BUCKET_ROLL_MAX) % EVENT_BUCKET_COUNT) {
                        case 0:
                        case 1:
                        case 2:
                            cell->m_objectMetadata = DAEMON_REWARD_EXPERIENCE;
                            break;
                        case 3:
                            cell->m_objectMetadata = DAEMON_REWARD_ARTIFACT;
                            break;
                        case 4:
                        case 5:
                        case 6:
                            cell->m_objectMetadata = DAEMON_REWARD_EXPERIENCE_GOLD;
                            break;
                        case 7:
                        case 8:
                        case 9:
                            cell->m_objectMetadata = DAEMON_REWARD_RANSOM;
                            break;
                    }
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_TREASURE_CHEST):
                    if (CELL_TERRAIN(cell) == TERRAIN_WATER) {
                        cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_SEA_CHEST);
                        randomValue = Random(EVENT_ROLL_MIN, EVENT_ROLL_MAX);
                        if (randomValue < SEA_CHEST_EMPTY_CUTOFF)
                            cell->m_objectMetadata = SEA_CHEST_OUTCOME_EMPTY;
                        else if (randomValue < SEA_CHEST_GOLD_CUTOFF)
                            cell->m_objectMetadata = SEA_CHEST_OUTCOME_GOLD;
                        else
                            cell->m_objectMetadata =
                                GetRandomArtifactId(ARTIFACT_LEVEL_TREASURE, true)
                                | CHEST_ARTIFACT_FLAG;
                    } else {
                        randomValue = Random(EVENT_ROLL_MIN, EVENT_ROLL_MAX);
                        if (randomValue < LAND_CHEST_SMALL_CUTOFF)
                            cell->m_objectMetadata = CHEST_REWARD_SMALL;
                        else if (randomValue < LAND_CHEST_MEDIUM_CUTOFF)
                            cell->m_objectMetadata = CHEST_REWARD_MEDIUM;
                        else if (randomValue < LAND_CHEST_LARGE_CUTOFF)
                            cell->m_objectMetadata = CHEST_REWARD_LARGE;
                        else
                            cell->m_objectMetadata =
                                GetRandomArtifactId(ARTIFACT_LEVEL_TREASURE, true)
                                | CHEST_ARTIFACT_FLAG;
                    }
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_CAMPFIRE):
                    cell->m_objectMetadata =
                        Random(CAMPFIRE_AMOUNT_MIN, CAMPFIRE_AMOUNT_MAX) << CAMPFIRE_AMOUNT_SHIFT;
                    cell->m_objectMetadata |= Random(IDX(RES_WOOD), IDX(RES_GEMS));
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_ANCIENT_LAMP):
                    cell->m_objectMetadata =
                        Random(ANCIENT_LAMP_ROLL_MIN, ANCIENT_LAMP_ROLL_MAX)
                        + ANCIENT_LAMP_COUNT_OFFSET;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_SHIPWRECK_SURVIVOR):
                    randomValue = Random(EVENT_ROLL_MIN, EVENT_ROLL_MAX);
                    if (randomValue < SHIPWRECK_SURVIVOR_TREASURE_CUTOFF)
                        cell->m_objectMetadata =
                            GetRandomArtifactId(ARTIFACT_LEVEL_TREASURE, true);
                    else if (randomValue < SHIPWRECK_SURVIVOR_MINOR_CUTOFF)
                        cell->m_objectMetadata = GetRandomArtifactId(ARTIFACT_LEVEL_MINOR, true);
                    else
                        cell->m_objectMetadata = GetRandomArtifactId(ARTIFACT_LEVEL_MAJOR, true);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_GRAVEYARD):
                case MAP_ACTION_TRIGGER(MAP_OBJECT_SHIPWRECK):
                case MAP_ACTION_TRIGGER(MAP_OBJECT_DERELICT_SHIP):
                    switch (Random(EVENT_ROLL_MIN, EVENT_BUCKET_ROLL_MAX) % EVENT_BUCKET_COUNT) {
                        case 0:
                        case 1:
                        case 2:
                            cell->m_objectMetadata = IDX(EVENT_LEVEL_SMALL);
                            break;
                        case 3:
                        case 4:
                        case 5:
                            cell->m_objectMetadata = IDX(EVENT_LEVEL_MEDIUM);
                            break;
                        case 6:
                        case 7:
                        case 8:
                            cell->m_objectMetadata = IDX(EVENT_LEVEL_LARGE);
                            break;
                        case 9:
                            cell->m_objectMetadata = IDX(EVENT_LEVEL_HUGE);
                            break;
                    }
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_ARCHER_HOUSE):
                    cell->m_objectMetadata = Random(10, 25);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_GOBLIN_HUT):
                    cell->m_objectMetadata = Random(15, 40);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_DWARF_COTTAGE):
                    cell->m_objectMetadata = Random(0, 20) + 1;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_PEASANT_HUT):
                    cell->m_objectMetadata = Random(0, 40) + 1;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_LOG_CABIN):
                    cell->m_objectMetadata = Random(20, 50);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_WATER_WHEEL):
                    cell->m_objectMetadata = MAP_EVENT_DATA_AVAILABLE;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_ARTESIAN_SPRING):
                    cell->m_objectMetadata = MAP_EVENT_DATA_AVAILABLE;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_MAGIC_GARDEN):
                    cell->m_objectMetadata =
                        Random(EVENT_ROLL_MIN, EVENT_BINARY_ROLL_MAX) == EVENT_ROLL_MIN
                            ? IDX(RES_GEMS) + MAP_EVENT_RESOURCE_OFFSET
                            : IDX(RES_GOLD) + MAP_EVENT_RESOURCE_OFFSET;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_TREE_OF_KNOWLEDGE):
                    cell->m_objectMetadata =
                        eyeId++
                        | (Random(TREE_KNOWLEDGE_FREE, TREE_KNOWLEDGE_GEMS)
                           << TREE_KNOWLEDGE_MODE_SHIFT);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER):
                    if (cell->m_objectMetadata == MAP_EVENT_DATA_EMPTY) {
                        cell->m_objectMetadata = GetRandomNumTroops(
                            static_cast<CreatureType>(cell->m_objectIndex)
                        );
                        if (cell->m_objectIndex != IDX(CREATURE_GHOST)
                            && cell->m_objectIndex != IDX(CREATURE_EARTH_ELEMENTAL)
                            && cell->m_objectIndex != IDX(CREATURE_AIR_ELEMENTAL)
                            && cell->m_objectIndex != IDX(CREATURE_FIRE_ELEMENTAL)
                            && cell->m_objectIndex != IDX(CREATURE_WATER_ELEMENTAL)
                            && Random(MONSTER_GUARD_ROLL_MIN, MONSTER_GUARD_ROLL_MAX)
                                   < MONSTER_GUARD_CUTOFF)
                            cell->m_objectMetadata |= IDX(MAP_MONSTER_GUARD_FLAG);
                    }
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE):
                    cell->m_objectMetadata = cell->m_objectIndex / 2;
                    switch (cell->m_objectMetadata) {
                        case IDX(RES_WOOD):
                        case IDX(RES_ORE):
                            cell->m_objectMetadata =
                                Random(RESOURCE_BULK_AMOUNT_MIN, RESOURCE_BULK_AMOUNT_MAX);
                            break;
                        case IDX(RES_GOLD):
                            cell->m_objectMetadata =
                                Random(RESOURCE_BULK_AMOUNT_MIN, RESOURCE_BULK_AMOUNT_MAX);
                            break;
                        default:
                            cell->m_objectMetadata =
                                Random(RESOURCE_SCARCE_AMOUNT_MIN, RESOURCE_SCARCE_AMOUNT_MAX);
                            break;
                    }
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_SHRINE_FIRST_CIRCLE):
                    cell->m_objectMetadata =
                        Random(IDX(SPELL_FIREBALL), IDX(SPELL_COUNT) - 1) + MAP_EVENT_SPELL_OFFSET;
                    while (gsSpellInfo[cell->m_objectMetadata - MAP_EVENT_SPELL_OFFSET].level
                           != SPELL_LEVEL_FIRST)
                        cell->m_objectMetadata = Random(IDX(SPELL_FIREBALL), IDX(SPELL_COUNT) - 1)
                                                  + MAP_EVENT_SPELL_OFFSET;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_SHRINE_SECOND_CIRCLE):
                    cell->m_objectMetadata =
                        Random(IDX(SPELL_FIREBALL), IDX(SPELL_COUNT) - 1) + MAP_EVENT_SPELL_OFFSET;
                    while (gsSpellInfo[cell->m_objectMetadata - MAP_EVENT_SPELL_OFFSET].level
                           != SPELL_LEVEL_SECOND)
                        cell->m_objectMetadata = Random(IDX(SPELL_FIREBALL), IDX(SPELL_COUNT) - 1)
                                                  + MAP_EVENT_SPELL_OFFSET;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_SHRINE_THIRD_CIRCLE):
                    cell->m_objectMetadata =
                        Random(IDX(SPELL_FIREBALL), IDX(SPELL_COUNT) - 1) + MAP_EVENT_SPELL_OFFSET;
                    while (gsSpellInfo[cell->m_objectMetadata - MAP_EVENT_SPELL_OFFSET].level
                           != SPELL_LEVEL_THIRD)
                        cell->m_objectMetadata = Random(IDX(SPELL_FIREBALL), IDX(SPELL_COUNT) - 1)
                                                  + MAP_EVENT_SPELL_OFFSET;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_PYRAMID):
                    cell->m_objectMetadata =
                        Random(IDX(SPELL_FIREBALL), IDX(SPELL_COUNT) - 1) + MAP_EVENT_SPELL_OFFSET;
                    while (gsSpellInfo[cell->m_objectMetadata - MAP_EVENT_SPELL_OFFSET].level
                           != SPELL_LEVEL_FIFTH)
                        cell->m_objectMetadata = Random(IDX(SPELL_FIREBALL), IDX(SPELL_COUNT) - 1)
                                                  + MAP_EVENT_SPELL_OFFSET;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_TREE_HOUSE):
                    cell->m_objectMetadata = Random(15, 25);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_SIRENS):
                    cell->m_objectMetadata = Random(10, 20);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_WATCH_TOWER):
                    cell->m_objectMetadata = Random(7, 10);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RUINS):
                    cell->m_objectMetadata = Random(3, 5);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_TREE_CITY):
                    cell->m_objectMetadata = Random(20, 40);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_HALFLING_HOLE):
                    cell->m_objectMetadata = Random(20, 40);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_TROLL_BRIDGE):
                    cell->m_objectMetadata = Random(4, 6) | BANK_GUARDIAN_FLAG;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_CITY_OF_DEAD):
                    cell->m_objectMetadata = Random(4, 6) | BANK_GUARDIAN_FLAG;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_DRAGON_CITY):
                    cell->m_objectMetadata = BANK_GUARDIAN_FLAG | 2;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_CAVE):
                    cell->m_objectMetadata = Random(10, 20);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_EXCAVATION):
                    cell->m_objectMetadata = Random(10, 25);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_DESERT_TENT):
                    cell->m_objectMetadata = Random(10, 20);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_WAGON_CAMP):
                    if (!HasObjectTilesetIndex(
                            xPosition,
                            yPosition,
                            TILESET_OBJNMUL2,
                            WAGON_CAMP_ACTIVE_FRAME
                        )) {
                        cell->m_triggerType &= MAP_TRIGGER_TYPE_MASK;
                        break;
                    }
                    cell->m_objectMetadata = Random(30, 50);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_ARTIFACT):
                    randomValue = Random(EVENT_ROLL_MIN, EVENT_BUCKET_ROLL_MAX);
                    value = static_cast<ArtifactType>(cell->m_objectIndex / 2);
                    if (value == ARTIFACT_SPELL_SCROLL)
                        break;
                    if (randomValue < ARTIFACT_EVENT_UNCONDITIONAL_CUTOFF) {
                        if (randomValue % EVENT_BUCKET_COUNT
                            == ARTIFACT_EVENT_WISDOM_BUCKET)
                            cell->m_objectMetadata = ARTIFACT_EVENT_MODE_WISDOM;
                        else if (randomValue % EVENT_BUCKET_COUNT
                                 == ARTIFACT_EVENT_LEADERSHIP_BUCKET)
                            cell->m_objectMetadata = ARTIFACT_EVENT_MODE_LEADERSHIP;
                        else
                            cell->m_objectMetadata = ARTIFACT_EVENT_MODE_PICKUP;
                    } else if (randomValue < ARTIFACT_EVENT_GUARD_CUTOFF) {
                        if (gArtifactLevel[IDX(value)] == ARTIFACT_LEVEL_TREASURE)
                            cell->m_objectMetadata = ARTIFACT_EVENT_MODE_GOLD;
                        else if (gArtifactLevel[IDX(value)] == ARTIFACT_LEVEL_MINOR)
                            cell->m_objectMetadata =
                                (Random(IDX(RES_WOOD), IDX(RES_GEMS))
                                 << ARTIFACT_EVENT_RESOURCE_SHIFT)
                                | ARTIFACT_EVENT_MODE_RESOURCE_3;
                        else if (gArtifactLevel[IDX(value)] == ARTIFACT_LEVEL_MAJOR)
                            cell->m_objectMetadata =
                                (Random(IDX(RES_WOOD), IDX(RES_GEMS))
                                 << ARTIFACT_EVENT_RESOURCE_SHIFT)
                                | ARTIFACT_EVENT_MODE_RESOURCE_5;
                    } else {
                        artifactGuardianChoices[0] = CREATURE_PALADIN;
                        artifactGuardianChoices[1] = CREATURE_CRUSADER;
                        artifactGuardianChoices[2] = CREATURE_CYCLOPS;
                        artifactGuardianChoices[3] = CREATURE_GENIE;
                        artifactGuardianChoices[4] = CREATURE_GREEN_DRAGON;
                        artifactGuardianChoices[5] = CREATURE_RED_DRAGON;
                        artifactGuardianChoices[6] = CREATURE_BLACK_DRAGON;
                        artifactGuardianChoices[7] = CREATURE_BONE_DRAGON;
                        artifactGuardianChoices[8] = CREATURE_GIANT;
                        artifactGuardianChoices[9] = CREATURE_TITAN;
                        cell->m_objectMetadata = ARTIFACT_EVENT_GUARDED_FLAG;
                        if (gArtifactLevel[IDX(value)] == ARTIFACT_LEVEL_TREASURE)
                            cell->m_objectMetadata |= IDX(CREATURE_ROGUE);
                        else if (gArtifactLevel[IDX(value)] == ARTIFACT_LEVEL_MINOR)
                            cell->m_objectMetadata |= IDX(artifactGuardianChoices[Random(
                                EVENT_ROLL_MIN,
                                MINOR_GUARDIAN_CHOICE_COUNT - 1
                            ) + MINOR_GUARDIAN_CHOICE_FIRST]);
                        else
                            cell->m_objectMetadata |= IDX(artifactGuardianChoices[Random(
                                EVENT_ROLL_MIN,
                                MAJOR_GUARDIAN_CHOICE_COUNT - 1
                            ) + MAJOR_GUARDIAN_CHOICE_FIRST]);
                    }
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE):
                    mineId = GetTownId(xPosition, yPosition);
                    for (row = yPosition - CASTLE_METADATA_TOP_OFFSET;
                         row <= yPosition + CASTLE_METADATA_BOTTOM_OFFSET;
                         row++) {
                        for (column = xPosition - CASTLE_METADATA_X_RADIUS;
                             column <= xPosition + CASTLE_METADATA_X_RADIUS;
                             column++) {
                            if (m_worldMap.GetCell(column, row)->m_objectMetadata == 0)
                                m_worldMap.GetCell(column, row)->m_objectMetadata = mineId;
                        }
                    }
                    townRec = GetCastleSlot(this, mineId);
                    townRec->m_boatY = -1;
                    townRec->m_boatX = -1;
                    if (yPosition <= MAP_HEIGHT - CASTLE_BOAT_Y_OFFSET - 1) {
                        townEntrance = gpAdvManager->GetCell(
                            xPosition - CASTLE_BOAT_X_OFFSET,
                            yPosition + CASTLE_BOAT_Y_OFFSET
                        );
                        if (CELL_TERRAIN(townEntrance) == TERRAIN_WATER) {
                            townRec->m_boatX = static_cast<i8>(xPosition - CASTLE_BOAT_X_OFFSET);
                            townRec->m_boatY = static_cast<i8>(yPosition + CASTLE_BOAT_Y_OFFSET);
                        } else {
                            townEntrance = gpAdvManager->GetCell(
                                xPosition + CASTLE_BOAT_X_OFFSET,
                                yPosition + CASTLE_BOAT_Y_OFFSET
                            );
                            if (CELL_TERRAIN(townEntrance) == TERRAIN_WATER) {
                                townRec->m_boatX =
                                    static_cast<i8>(xPosition + CASTLE_BOAT_X_OFFSET);
                                townRec->m_boatY =
                                    static_cast<i8>(yPosition + CASTLE_BOAT_Y_OFFSET);
                            }
                        }
                    }
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_LIGHTHOUSE): {
                    m_worldMap.GetCell(xPosition, yPosition)->m_objectMetadata = GetMineId(xPosition, yPosition);
                    break;
                }
                case MAP_ACTION_TRIGGER(MAP_OBJECT_ABANDONED_MINE):
                    mineId = GetMineId(xPosition, yPosition);
                    m_mines[mineId].guardianType = CREATURE_GHOST;
                    m_mines[mineId].guardianCount = static_cast<u8>(Random(
                        ABANDONED_MINE_GUARDIAN_COUNT_MIN,
                        ABANDONED_MINE_GUARDIAN_COUNT_MAX
                    ));
                case MAP_ACTION_TRIGGER(MAP_OBJECT_ALCHEMIST_LAB):
                case MAP_ACTION_TRIGGER(MAP_OBJECT_MINE):
                case MAP_ACTION_TRIGGER(MAP_OBJECT_SAWMILL):
                    mineId = GetMineId(xPosition, yPosition);
                    for (row = yPosition - MINE_METADATA_TOP_OFFSET; row <= yPosition;
                         row++) {
                        for (column = xPosition - MINE_METADATA_LEFT_OFFSET;
                             column <= xPosition + MINE_METADATA_RIGHT_OFFSET;
                             column++) {
                            if (column == xPosition - MINE_METADATA_LEFT_OFFSET
                                && cell->m_triggerType
                                       != (MAP_ACTION_TRIGGER(MAP_OBJECT_ALCHEMIST_LAB)))
                                continue;
                            if (m_worldMap.GetCell(column, row)->m_objectMetadata == 0
                                || ((m_worldMap.GetCell(column, row)->m_triggerType
                                     & MAP_TRIGGER_TYPE_MASK)
                                    == (cell->m_triggerType & MAP_TRIGGER_TYPE_MASK)))
                                m_worldMap.GetCell(column, row)->m_objectMetadata = mineId;
                        }
                    }
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_WINDMILL):
                    cell->m_objectMetadata =
                        Random(WINDMILL_RESOURCE_AMOUNT_MIN, WINDMILL_RESOURCE_AMOUNT_MAX);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_BARRIER):
                    RandomizeBarrier(cell);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_TRAVELER_TENT):
                    RandomizePassword(cell);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_EXPANSION_OBJECT):
                    WeeklyGenericSite(cell);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_EXPANSION_DWELLING):
                    WeeklyRecruitSite(cell);
                    break;
            }
        }
    }

    for (yPosition = 0; yPosition < MAP_HEIGHT; yPosition++) {
        for (xPosition = 0; xPosition < MAP_WIDTH; xPosition++) {
            cell = m_worldMap.GetCell(xPosition, yPosition);
            if (cell->m_objectIndex != MAPCELL_SPRITE_NONE && cell->m_objectLayerBit1) {
                valid = true;
                extraIndex = cell->m_extraIndex;
                while (extraIndex != 0) {
                    extra = m_worldMap.Extra(extraIndex);
                    if (extra->objectIndex != MAPCELL_SPRITE_NONE
                        && !extra->objectLayerBit1)
                        valid = false;
                    extraIndex = extra->nextIndex;
                }
                if (valid)
                    cell->m_flags |= IDX(MAP_CELL_OBJECT_SHADOW_ONLY);
            }
        }
    }

    for (yPosition = 0; yPosition < MAP_HEIGHT; yPosition++) {
        for (xPosition = 0; xPosition < MAP_WIDTH; xPosition++) {
            cell = m_worldMap.GetCell(xPosition, yPosition);
            if ((cell->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_ROCK
                && cell->m_objectTileset == TILESET_X_LOC2)
                cell->m_flags |= IDX(MAP_CELL_OCCUPIED);
            if (cell->m_objectIndex != MAPCELL_SPRITE_NONE
                && !(cell->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell->m_flags & IDX(MAP_CELL_OBJECT_SHADOW_ONLY))
                && cell->m_overlayIndex != MAPCELL_SPRITE_NONE)
                cell->m_flags |= IDX(MAP_CELL_OCCUPIED);
            upperCount = 0;
            lowerCount = 0;
            if (!(cell->m_flags & IDX(MAP_CELL_OCCUPIED)) && yPosition < MAP_HEIGHT - 1
                && cell->m_objectIndex != MAPCELL_SPRITE_NONE
                && !(cell->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell->m_flags & IDX(MAP_CELL_OBJECT_SHADOW_ONLY))) {
                mapCell* below;
                if (m_worldMap.GetCell(xPosition, yPosition + 1)->m_objectIndex != MAPCELL_SPRITE_NONE
                    && !(
                        m_worldMap.GetCell(xPosition, yPosition + 1)->m_triggerType
                        & MAP_TRIGGER_ACTION_FLAG
                    )
                    && !(
                        m_worldMap.GetCell(xPosition, yPosition + 1)->m_flags
                        & IDX(MAP_CELL_OBJECT_SHADOW_ONLY)
                    )) {
                    if (!cell->m_objectLayerBit1) {
                        upperTilesets[upperCount] = cell->m_objectTileset;
                        upperIndexes[upperCount] = cell->m_objectIndex;
                        upperCount++;
                    }
                    if (cell->m_extraIndex != 0)
                        extra = m_worldMap.Extra(cell->m_extraIndex);
                    else
                        extra = NULL;
                    while (upperCount < LAYER_SCAN_CAPACITY && extra != NULL) {
                        if (extra->objectIndex != MAPCELL_SPRITE_NONE
                            && !extra->objectLayerBit1) {
                            upperTilesets[upperCount] = extra->objectTileset;
                            upperIndexes[upperCount] = extra->objectIndex;
                            upperCount++;
                        }
                        if (extra->nextIndex != 0)
                            extra = m_worldMap.Extra(extra->nextIndex);
                        else
                            extra = NULL;
                    }
                    below = m_worldMap.GetCell(xPosition, yPosition + 1);
                    if (!below->m_objectLayerBit1) {
                        lowerTilesets[lowerCount] = below->m_objectTileset;
                        lowerIndexes[lowerCount] = below->m_objectIndex;
                        lowerCount++;
                    }
                    if (below->m_extraIndex != 0)
                        extra = m_worldMap.Extra(below->m_extraIndex);
                    else
                        extra = NULL;
                    while (lowerCount < LAYER_SCAN_CAPACITY && extra != NULL) {
                        if (extra->objectIndex != MAPCELL_SPRITE_NONE
                            && !extra->objectLayerBit1) {
                            lowerTilesets[lowerCount] = extra->objectTileset;
                            lowerIndexes[lowerCount] = extra->objectIndex;
                            lowerCount++;
                        }
                        if (extra->nextIndex != 0)
                            extra = m_worldMap.Extra(extra->nextIndex);
                        else
                            extra = NULL;
                    }
                    for (randomValue = 0; randomValue < upperCount; randomValue++) {
                        for (lowerTileIndex = 0; lowerTileIndex < lowerCount; lowerTileIndex++) {
                            if (upperTilesets[randomValue] == lowerTilesets[lowerTileIndex]
                                || (upperTilesets[randomValue] >= TILESET_OBJNTOWN
                                    && upperTilesets[randomValue] <= TILESET_OBJNTWRD
                                    && lowerTilesets[lowerTileIndex] >= TILESET_OBJNTOWN
                                    && lowerTilesets[lowerTileIndex] <= TILESET_OBJNTWRD))
                                cell->m_flags |= IDX(MAP_CELL_OCCUPIED);
                        }
                    }
                }
            }
            if (yPosition < MAP_HEIGHT - 1) {
                if (m_worldMap.GetCell(xPosition, yPosition + 1)->m_triggerType
                        == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
                    || m_worldMap.GetCell(xPosition, yPosition + 1)->m_triggerType
                           == (MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_TOWN))
                    || m_worldMap.GetCell(xPosition, yPosition + 1)->m_triggerType
                           == (MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_CASTLE)))
                    cell->m_flags |= IDX(MAP_CELL_OCCUPIED);
            }
            if (cell->m_objectIndex != MAPCELL_SPRITE_NONE
                && !(cell->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell->m_flags & IDX(MAP_CELL_OBJECT_SHADOW_ONLY))
                && (yPosition == MAP_HEIGHT - 1
                    || (m_worldMap.GetCell(xPosition, yPosition + 1)->m_flags & 4)))
                cell->m_flags |= IDX(MAP_CELL_OCCUPIED);
        }
    }
}
#if H2_RETAIL_COMPILER
#undef artifactGuardianChoices
#undef below
#undef cell
#undef column
#undef eventData
#undef extra
#undef extraIndex
#undef eyeId
#undef hutId
#undef jailId
#undef lowerIndexes
#undef lowerTileIndex
#undef lowerTilesets
#undef mapEvent
#undef mineId
#undef randomValue
#undef shrineId
#undef tentId
#undef townEntrance
#undef upperCount
#undef upperIndexes
#undef upperTilesets
#undef xPosition
#undef yPosition
#endif

VA(0x00453a95, 0x85)
void game::InitializePasswords(void) {
    bchar flag;
    i32 i;
    i32 j;
    for (i = 0; i < PASSWORD_INDEX_COUNT; i++) {
        flag = false;
        while (flag == 0) {
            xPasswordStringsIndex[i] = Random(0, X_GLOBAL_PASSWORD_STRING_COUNT - 1);
            flag = true;
            for (j = 0; j < i; j++) {
                if (xPasswordStringsIndex[i] == xPasswordStringsIndex[j])
                    flag = false;
            }
        }
    }
}

#if H2_RETAIL_COMPILER
#define passwordIndex p
#endif
VA(0x00453b1a, 0x6b)
void game::RandomizeBarrier(mapCell* cell) {
    i32 index = cell->m_objectMetadata;
    index &= PASSWORD_INDEX_MASK;
    i32 passwordIndex = xPasswordStringsIndex[index];
    i32 color = (passwordIndex << PASSWORD_COLOR_SHIFT) | index;
    cell->m_objectMetadata = color;
}
#if H2_RETAIL_COMPILER
#undef passwordIndex
#endif

VA(0x00453b85, 0x19)
void game::RandomizePassword(mapCell* cell) {
    RandomizeBarrier(cell);
}

VA(0x00453b9e, 0x435)
i32 game::LoadMap(char* filename) {
    char x[LOAD_MAP_COORDINATE_SCRATCH_SIZE];
    char y[LOAD_MAP_COORDINATE_SCRATCH_SIZE];
    char type[LOAD_MAP_RECORD_SCRATCH_SIZE];
    char junk[LOAD_MAP_RECORD_SCRATCH_SIZE];
    i32 i;
    i32 handle;

    sprintf(gText, "%s%s", gcMapPath, filename);
    handle = open(gText, _O_BINARY);
    if (handle == -1)
        FileError(gText);
    READ_FILE_VALUE(handle, m_mapHeader);
    m_worldMap.Read(handle, 1);
    SetMapSize(m_worldMap.width, m_worldMap.height);

    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        read(handle, x, sizeof(x[0]));
        read(handle, y, sizeof(y[0]));
        read(handle, type, sizeof(type[0]));
        if (static_cast<u8>(x[0]) != SAVED_TOWN_OFF_MAP) {
            m_castleRecs[i].m_onMap = 1;
            m_castleRecs[i].m_x = static_cast<u8>(x[0]);
            m_castleRecs[i].m_y = static_cast<u8>(y[0]);
            m_castleRecs[i].m_type =
                static_cast<FactionType>(type[0] & TOWN_RECORD_TYPE_MASK);
            if (type[0] < 0)
                m_castleRecs[i].m_buildings |= IDX(TOWN_BUILDING_CASTLE);
            else
                m_castleRecs[i].m_buildings |= IDX(TOWN_BUILDING_TENT);
        }
    }

    for (i = 0; i < GAME_MINE_COUNT; i++) {
        if (m_mapHeader.magic == MAP_HEADER_MAGIC_BASE_GAME && i >= GAME_TOWN_COUNT) {
            x[0] = -1;
            y[0] = -1;
            type[0] = -1;
        } else {
            read(handle, x, sizeof(x[0]));
            read(handle, y, sizeof(y[0]));
            read(handle, type, sizeof(type[0]));
        }
        if (static_cast<u8>(x[0]) != SAVED_TOWN_OFF_MAP) {
            m_mines[i].guardianType = CREATURE_NONE;
            m_mines[i].x = static_cast<u8>(x[0]);
            m_mines[i].y = static_cast<u8>(y[0]);
            m_mines[i].resourceType = static_cast<MineType>(type[0]);
        }
    }

    m_mapHeader.magic = MAP_HEADER_MAGIC_EXPANSION_GAME;
    READ_FILE_VALUE(handle, m_obeliskCount);
    read(
        handle,
        m_rumourEventIndices,
        m_mapHeader.rumourCount * sizeof(m_rumourEventIndices[0])
    );
    m_rumourEventCount = m_mapHeader.rumourCount;
    read(
        handle,
        m_timeEventIndices,
        m_mapHeader.timeEventCount * sizeof(m_timeEventIndices[0])
    );
    m_timeEventCount = m_mapHeader.timeEventCount;
    READ_FILE_VALUE(handle, iMaxMapExtra);
    ppMapExtra = reinterpret_cast<void**>(
        H2_ALLOC(iMaxMapExtra * sizeof(ppMapExtra[0]))
    );
    pwSizeOfMapExtra = reinterpret_cast<i16*>(
        H2_ALLOC(iMaxMapExtra * sizeof(pwSizeOfMapExtra[0]))
    );
    memset(ppMapExtra, 0, iMaxMapExtra * sizeof(ppMapExtra[0]));
    memset(pwSizeOfMapExtra, 0, iMaxMapExtra * sizeof(pwSizeOfMapExtra[0]));
    for (i = 1; i < iMaxMapExtra; i++) {
        read(handle, pwSizeOfMapExtra + i, sizeof(pwSizeOfMapExtra[0]));
        ppMapExtra[i] = H2_ALLOC(pwSizeOfMapExtra[i]);
        read(handle, ppMapExtra[i], pwSizeOfMapExtra[i]);
    }
    read(handle, junk, sizeof(u16));
    close(handle);
    return 0;
}

#if H2_RETAIL_COMPILER
#define cellPointer cellPtr8
#define townRec townRec4
#endif
VA(0x00453fd3, 0x307)
void game::ClaimTown(i32 townId, i32 player, i32 suppressVisibility) {
    i32 i;
    town* townRec;
    mapCell* cellPointer;

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
    townRec = &m_castleRecs[townId];
    if (townRec->m_owner == player)
        return;
    townRec->m_formation = TOWN_FORMATION_SPREAD;
    if (m_castleOwners[townId] != -1)
        GetCastle(townId)->Deallocate();
    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        townRec->m_army.m_creatureTypes[i] = CREATURE_NONE;
        townRec->m_army.m_creatureCounts[i] = 0;
    }
    if (m_castleRecs[townId].m_owner == -1)
        m_castleRecs[townId].m_turnsOwned = TOWN_NEW_OWNER_TURN_COUNT;
    else
        m_castleRecs[townId].m_turnsOwned = 0;
    m_castleRecs[townId].m_owner = static_cast<i8>(player);
    m_castleOwners[townId] = static_cast<i8>(player);
    m_players[player].m_townIds[m_players[player].m_townCount] = static_cast<i8>(townId);
    m_players[player].m_townCount++;

    cellPointer = m_worldMap.GetCell(m_castleRecs[townId].m_x - 1, m_castleRecs[townId].m_y);
    m_worldMap.ChangeTilesetIndex(
        cellPointer,
        m_castleRecs[townId].m_x - 1,
        m_castleRecs[townId].m_y,
        TILESET_FLAG32,
        GetPlayerColor(static_cast<i8>(player)) * TOWN_FLAG_FRAME_STRIDE,
        1,
        -1
    );
    cellPointer = m_worldMap.GetCell(m_castleRecs[townId].m_x + 1, m_castleRecs[townId].m_y);
    m_worldMap.ChangeTilesetIndex(
        cellPointer,
        m_castleRecs[townId].m_x + 1,
        m_castleRecs[townId].m_y,
        TILESET_FLAG32,
        GetPlayerColor(static_cast<i8>(player)) * TOWN_FLAG_FRAME_STRIDE
            + TOWN_FLAG_RIGHT_FRAME,
        1,
        -1
    );
    if (suppressVisibility != 0)
        return;
    SetVisibility(m_castleRecs[townId].m_x, m_castleRecs[townId].m_y, player, giVisRangeTown);
    CheckEndGame(END_GAME_FORCE_NONE, false);
}
#if H2_RETAIL_COMPILER
#undef cellPointer
#undef townRec
#endif

#if H2_RETAIL_COMPILER
#define flag flag1
#endif
VA(0x004542da, 0x414)
void game::ClaimMine(i32 mineId, i32 player) {
    i32 x;
    i32 y;
    i32 flag;
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
            flag = MINE_FLAG_ALCHEMIST_OFFSET;
            break;
        case MINE_TYPE_LIGHTHOUSE:
            flag = MINE_FLAG_LIGHTHOUSE_OFFSET;
            break;
        case MINE_TYPE_WOOD:
            flag = MINE_FLAG_WOOD_OFFSET;
            break;
        case MINE_TYPE_MERCURY:
            flag = MINE_FLAG_MERCURY_OFFSET;
            break;
        default:
            flag = MINE_FLAG_COMMON_OFFSET;
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
            flag + GetPlayerColor(static_cast<i8>(player)),
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
#if H2_RETAIL_COMPILER
#undef flag
#endif

VA(0x004546ee, 0x1e1)
SpellType
game::ViewSpells(
    hero* spellHero, HeroSpellType spellType, MessageDispatchHandler callback, i32 readOnly
) {
    tag_message message;

    viewSpellsHero = spellHero;
    m_viewSpell = SPELL_NONE;
    if (spellHero->GetNumSpells(spellType) == 0) {
        NormalDialog(
            const_cast<char*>(localization::Tr("spell.none_to_cast")),
            1
        );
    } else {
        m_viewSpellsCallback = callback;
        m_viewSpellsReadOnly = static_cast<i8>(readOnly);
        m_viewSpellsHero = spellHero;
        if (spellType == SPELL_TYPE_ALL)
            m_viewSpellsType = SPELL_TYPE_ADVENTURE;
        else
            m_viewSpellsType = spellType;
        m_viewSpellsTop[IDX(SPELL_TYPE_COMBAT)] = 0;
        m_viewSpellsCount[IDX(SPELL_TYPE_COMBAT)] =
            spellHero->GetNumSpells(SPELL_TYPE_COMBAT);
        m_viewSpellsTop[IDX(SPELL_TYPE_ADVENTURE)] = 0;
        m_viewSpellsCount[IDX(SPELL_TYPE_ADVENTURE)] =
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
            message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
            m_viewSpellsWindow->BroadcastMessage(message);
        }
        UpdateSpellWidgets();
        gpWindowManager->DoDialog(m_viewSpellsWindow, ViewSpellsHandler, 0);
        delete m_viewSpellsWindow;
    }
    return m_viewSpell;
}

#if H2_RETAIL_COMPILER
#define spell spell1
#define spellPoints spellPoints0
#endif
VA(0x004548cf, 0x48f)
void game::UpdateSpellWidgets(void) {
    i32 spellPoints;
    i32 lines;
    tag_message message;
    i32 i;
    SpellType spell;

    message.type = MESSAGE_WIDGET;
    spellPoints = m_viewSpellsHero->m_spellPoints;
    if (spellPoints > VIEW_SPELL_MANA_MAX)
        spellPoints = VIEW_SPELL_MANA_MAX;
    memset(&message, 0, sizeof(message));

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = static_cast<BaseWidgetCommand>(
        IDX(WIDGET_COMMAND_SET_FLAGS)
        + static_cast<i32>(spellPoints <= VIEW_SPELL_MANA_HUNDREDS_THRESHOLD)
    );
    message.payload.widget.id = VIEW_SPELL_MANA_HUNDREDS_ID;
    message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
    m_viewSpellsWindow->BroadcastMessage(message);

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = static_cast<BaseWidgetCommand>(
        IDX(WIDGET_COMMAND_SET_FLAGS)
        + static_cast<i32>(spellPoints <= VIEW_SPELL_MANA_TENS_THRESHOLD)
    );
    message.payload.widget.id = VIEW_SPELL_MANA_TENS_ID;
    message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
    m_viewSpellsWindow->BroadcastMessage(message);

    sprintf(
        gText,
        "%d",
        (spellPoints / VIEW_SPELL_MANA_HUNDREDS_DIVISOR) % VIEW_SPELL_MANA_DIGIT_BASE
    );
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, VIEW_SPELL_MANA_HUNDREDS_ID);
    message.payload.widget.data.text = gText;
    m_viewSpellsWindow->BroadcastMessage(message);

    sprintf(
        gText,
        "%d",
        (spellPoints / VIEW_SPELL_MANA_TENS_DIVISOR) % VIEW_SPELL_MANA_DIGIT_BASE
    );
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, VIEW_SPELL_MANA_TENS_ID);
    message.payload.widget.data.text = gText;
    m_viewSpellsWindow->BroadcastMessage(message);

    sprintf(gText, "%d", spellPoints % VIEW_SPELL_MANA_DIGIT_BASE);
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, VIEW_SPELL_MANA_ONES_ID);
    message.payload.widget.data.text = gText;
    m_viewSpellsWindow->BroadcastMessage(message);

    for (i = 0; i < VIEW_SPELL_PAGE_SIZE; i++) {
        if (m_viewSpellsTop[IDX(m_viewSpellsType)] + i
            >= m_viewSpellsCount[IDX(m_viewSpellsType)]) {
            SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_CLEAR_FLAGS, i + VIEW_SPELL_ICON_ID_BASE);
            message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
            m_viewSpellsWindow->BroadcastMessage(message);

            SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_CLEAR_FLAGS, i + VIEW_SPELL_TEXT_ID_BASE);
            message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
            m_viewSpellsWindow->BroadcastMessage(message);
        } else {
            SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_FLAGS, i + VIEW_SPELL_TEXT_ID_BASE);
            message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED);
            m_viewSpellsWindow->BroadcastMessage(message);

            spell = m_viewSpellsHero->GetNthSpell(
                m_viewSpellsType,
                m_viewSpellsTop[IDX(m_viewSpellsType)] + i + 1
            );
            SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_FILL_COLOR, i + VIEW_SPELL_TEXT_ID_BASE);
            if (GetManaCost(spell, m_viewSpellsHero) > m_viewSpellsHero->m_spellPoints)
                message.payload.widget.data.value = VIEW_SPELL_UNAVAILABLE_COLOR;
            else
                message.payload.widget.data.value = VIEW_SPELL_AVAILABLE_COLOR;
            m_viewSpellsWindow->BroadcastMessage(message);

            lines = smallFont->LineLength(gSpellNames[IDX(spell)], VIEW_SPELL_NAME_WIDTH);
            if (lines == 1) {
                sprintf(
                    gText,
                    "%s\n[%d]",
                    gSpellNames[IDX(spell)],
                    GetManaCost(spell, m_viewSpellsHero)
                );
            } else {
                sprintf(
                    gText,
                    "%s [%d]",
                    gSpellNames[IDX(spell)],
                    GetManaCost(spell, m_viewSpellsHero)
                );
            }
            SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, i + VIEW_SPELL_TEXT_ID_BASE);
            message.payload.widget.data.text = gText;
            m_viewSpellsWindow->BroadcastMessage(message);

            SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_FLAGS, i + VIEW_SPELL_TEXT_ID_BASE);
            message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
            m_viewSpellsWindow->BroadcastMessage(message);

            SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_FLAGS, i + VIEW_SPELL_ICON_ID_BASE);
            message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED);
            m_viewSpellsWindow->BroadcastMessage(message);

            SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_FRAME, i + VIEW_SPELL_ICON_ID_BASE);
            message.payload.widget.data.value = gsSpellInfo[IDX(spell)].iconIndex;
            m_viewSpellsWindow->BroadcastMessage(message);

            SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_FLAGS, i + VIEW_SPELL_ICON_ID_BASE);
            message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
            m_viewSpellsWindow->BroadcastMessage(message);
        }
    }
}
#if H2_RETAIL_COMPILER
#undef spell
#undef spellPoints
#endif

VA(0x00454d5e, 0x673)
MessageDispatchResult ViewSpellsHandler(tag_message& message) {
    SpellType spell;

    if (message.type == MESSAGE_MOUSE_MOVE) {
        gpWindowManager->ConvertToHover(message);
        if (gpWindowManager->m_lastHoverId == message.payload.hover.id) {
            return MESSAGE_DISPATCH_CONSUME;
        } else {
            return gpGame->m_viewSpellsCallback(message);
        }
    }
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                if (message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT
                    || (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) != 0)
                    break;
                {
                    switch (message.payload.widget.id) {
                        case VIEW_SPELL_MANA_LABEL_ID:
                        case VIEW_SPELL_MANA_HUNDREDS_ID:
                        case VIEW_SPELL_MANA_TENS_ID:
                        case VIEW_SPELL_MANA_ONES_ID:
                            sprintf(
                                gText,
                                cSpellHelp[VIEW_SPELL_HELP_MANA],
                                viewSpellsHero->m_spellPoints
                            );
                            NormalDialog(gText, NORMAL_DIALOG_INFO);
                            break;
                        case VIEW_SPELL_PREVIOUS_ID:
                            if (gpGame->m_viewSpellsTop[IDX(gpGame->m_viewSpellsType)] == 0) {
                                gpGame->UpdateSpellWidgets();
                                gpGame->m_viewSpellsWindow->MoveWindow(0, 0);
                                break;
                            }
                            gpGame->m_viewSpellsTop[IDX(gpGame->m_viewSpellsType)] -=
                                VIEW_SPELL_PAGE_SIZE;
                            if (gpGame->m_viewSpellsTop[IDX(gpGame->m_viewSpellsType)] < 0)
                                gpGame->m_viewSpellsTop[IDX(gpGame->m_viewSpellsType)] = 0;
                            gpGame->UpdateSpellWidgets();
                            gpGame->m_viewSpellsWindow->MoveWindow(0, 0);
                            break;
                        case VIEW_SPELL_NEXT_ID:
                            if (gpGame->m_viewSpellsTop[IDX(gpGame->m_viewSpellsType)]
                                    + VIEW_SPELL_PAGE_SIZE
                                < gpGame->m_viewSpellsCount[IDX(gpGame->m_viewSpellsType)])
                                gpGame->m_viewSpellsTop[IDX(gpGame->m_viewSpellsType)] +=
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
                            message.payload.widget.id = VIEW_SPELL_CLOSE_ID;
                            break;
                    }
                }
                break;
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                if (message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT
                    || (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) != 0) {
                    switch (message.payload.widget.id) {
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
                                gpGame->m_viewSpellsTop[IDX(gpGame->m_viewSpellsType)]
                                    + (message.payload.widget.id - VIEW_SPELL_ICON_ID_BASE) + 1
                            );
                            NormalDialog(
                                gSpellDesc[IDX(spell)],
                                NORMAL_DIALOG_QUICK_VIEW,
                                -1,
                                -1,
                                NORMAL_DIALOG_SPELL,
                                IDX(spell),
                                -1,
                                0,
                                -1,
                                0
                            );
                            break;
                        case VIEW_SPELL_PREVIOUS_ID:
                            NormalDialog(
                                cSpellHelp[VIEW_SPELL_HELP_PREVIOUS],
                                NORMAL_DIALOG_QUICK_VIEW
                            );
                            break;
                        case VIEW_SPELL_NEXT_ID:
                            NormalDialog(
                                cSpellHelp[VIEW_SPELL_HELP_NEXT],
                                NORMAL_DIALOG_QUICK_VIEW
                            );
                            break;
                        case VIEW_SPELL_COMBAT_TAB_ID:
                            NormalDialog(
                                cSpellHelp[VIEW_SPELL_HELP_COMBAT],
                                NORMAL_DIALOG_QUICK_VIEW
                            );
                            break;
                        case VIEW_SPELL_ADVENTURE_TAB_ID:
                            NormalDialog(
                                cSpellHelp[VIEW_SPELL_HELP_ADVENTURE],
                                NORMAL_DIALOG_QUICK_VIEW
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
                            NormalDialog(gText, NORMAL_DIALOG_QUICK_VIEW);
                            break;
                    }
                } else {
                    switch (message.payload.widget.id) {
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
                                gpGame->m_viewSpellsTop[IDX(gpGame->m_viewSpellsType)]
                                    + (message.payload.widget.id - VIEW_SPELL_ICON_ID_BASE) + 1
                            );
                            if (gpGame->m_viewSpellsReadOnly) {
                                NormalDialog(
                                    gSpellDesc[IDX(spell)],
                                    NORMAL_DIALOG_INFO,
                                    -1,
                                    -1,
                                    NORMAL_DIALOG_SPELL,
                                    IDX(spell),
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
                                NormalDialog(gText, NORMAL_DIALOG_INFO);
                                return MESSAGE_DISPATCH_CONTINUE;
                            }
                            gpGame->m_viewSpell = spell;
                            message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
                            return MESSAGE_DISPATCH_FORWARD;
                    }
                }
                break;
            default:
                break;
        }

        if (message.payload.widget.id == VIEW_SPELL_CLOSE_ID) {
            message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
            return MESSAGE_DISPATCH_FORWARD;
        }
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x004553d1, 0x158)
MessageDispatchResult ViewSpecialHandler(tag_message& message) {
    if (message.type == MESSAGE_MOUSE_MOVE) {
        if (message.payload.hover.id == gpWindowManager->m_lastHoverId)
            return MESSAGE_DISPATCH_CONSUME;
        gpWindowManager->m_lastHoverId = message.payload.hover.id;
        switch (message.payload.hover.id) {
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

#if H2_RETAIL_COMPILER
#define armyMonster armyMonster2
#define armyName armyName0
#define baseX baseX8
#define detailMessage detailMessage0
#define details details0
#define filename filename5
#define iconFrame iconFrame8
#define luck luck4
#define modifier modifier14
#define monsterIcon monsterIcon5
#define monsterWidget monsterWidget9
#define numWidget numWidget5
#define remainingShots shots8
#define spacing spacing5
#define spellCenterX spellCenterX27
#define spellIndex spellIndex8
#define spellWidget spellWidget0
#define spellX spellX2
#define spellY spellY10
#define titleMessage titleMessage14
#endif
VA(0x00455529, 0xd08)
void game::ViewArmy(
    i32 x,
    i32 y,
    H2_ENUM_PARAM(CreatureType, i32) monsterType,
    i32 numTroops,
    town* castle,
    i32 disableUpgrade,
    H2_ENUM_PARAM(ArmyFacing, i32) facing,
    i32 quickView,
    hero* theHero,
    class army* theArmy,
    armyGroup* theGroup,
    i32 groupIndex
) {
    i16 H2_UNUSED(titleMessage);
    i16 H2_UNUSED(detailMessage);
    tag_monsterInfo* monster;
    iconWidget* monsterWidget;
    char* details;
    i16 H2_UNUSED(blankWidget);
    i32 modifier;
    i16 H2_UNUSED(quickBaseY);
    tag_monsterInfo* armyMonster;
    char filename[VIEW_ARMY_FILENAME_SIZE];
    i16 H2_UNUSED(baseX);
    i32 loopIndex;
    i32 iconFrame;
    i16 H2_UNUSED(numWidget);
    i32 morale;
    i32 luck;
    u8 armyName[VIEW_ARMY_NAME_SIZE];
    icon* monsterIcon;
    tag_message message;
    i16 H2_UNUSED(frame);

    baseX = VIEW_ARMY_UNUSED_BASE_X;
    quickBaseY = VIEW_ARMY_UNUSED_QUICK_BASE_Y;
    blankWidget = VIEW_ARMY_BLANK_WIDGET_ID;
    numWidget = VIEW_ARMY_COUNT_WIDGET_ID;
    titleMessage = VIEW_ARMY_TITLE_WIDGET_ID;
    detailMessage = VIEW_ARMY_DETAIL_WIDGET_ID;
    frame = VIEW_ARMY_MONSTER_WIDGET_ID;
    message.type = MESSAGE_WIDGET;

    iViewArmyFrame = 0;
    iViewArmyType = monsterType;
    iViewArmyNumTroops = numTroops;
    gbAllowUpgrade = false;

    if (castle && (gpAdvManager->m_active == 1 || gpTownManager->m_active == 1)) {
        for (loopIndex = IDX(BUILDING_SLOT_DWELLING_SECOND);
             loopIndex <= IDX(BUILDING_SLOT_DWELLING_SIXTH);
             loopIndex++) {
            if (gDwellingType[IDX(castle->m_type)][loopIndex - IDX(BUILDING_SLOT_DWELLING_FIRST)]
                    == monsterType
                && HAS(
                    castle->m_buildings,
                    (1 << (loopIndex + VIEW_ARMY_DWELLING_UPGRADE_OFFSET))
                )) {
                gbAllowUpgrade = true;
                iViewArmyUpgradeToType = NextCreatureType(monsterType);
            }
        }
        if ((monsterType == CREATURE_GREEN_DRAGON || monsterType == CREATURE_RED_DRAGON)
            && HAS(castle->m_buildings, IDX(KB_DWELLING_UPGRADE_SIXTH_FLAG))) {
            gbAllowUpgrade = true;
            iViewArmyUpgradeToType = CREATURE_BLACK_DRAGON;
        }
    }

    monster = &gMonsterDatabase[IDX(monsterType)];
    if (theArmy)
        armyMonster = &theArmy->m_monster;
    else
        armyMonster = &gMonsterDatabase[IDX(monsterType)];

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
        ->PointToFile(gpResourceManager->MakeId(cArmyFrameFileNames[IDX(monsterType)], 1));
    gpResourceManager->ReadBlock(
        reinterpret_cast<i8*>(&sViewArmyMonFrameInfo),
        sizeof(sViewArmyMonFrameInfo)
    );
    ModifyFrameInfo(&sViewArmyMonFrameInfo, monsterType);
    BuildTempWalkSeq(&sViewArmyMonFrameInfo, 0, 1);

    viewArmyBaseX = VIEW_ARMY_MONSTER_BASE_X;
    strcpy(filename, cMonFilename[IDX(monsterType)]);

    monsterIcon = gpResourceManager->GetIcon(filename);
    iconFrame = sViewArmyMonFrameInfo.animationFrames[IDX(ARMY_ANIMATION_WALK)][0];
    viewArmyBaseX += viewArmyFacingWIPXMod
                     * (GetIconEntry(monsterIcon, iconFrame)->w
                        / VIEW_ARMY_ICON_CENTER_DIVISOR);
    viewArmyBaseX += GetIconEntry(monsterIcon, iconFrame)->x * viewArmyFacingWIPXMod
                     + viewArmyFacingWIPXMod * sViewArmyMonFrameInfo.walkXOffsets[0];
    viewArmyBaseY = VIEW_ARMY_MONSTER_BASE_Y;
    viewArmyBaseY +=
        GetIconEntry(monsterIcon, iconFrame)->h / VIEW_ARMY_ICON_CENTER_DIVISOR;
    monsterWidget = new iconWidget(
        static_cast<i16>(viewArmyBaseX),
        static_cast<i16>(viewArmyBaseY),
        VIEW_ARMY_MONSTER_WIDGET_WIDTH,
        VIEW_ARMY_MONSTER_WIDGET_HEIGHT,
        filename,
        sViewArmyMonFrameInfo.animationFrames[IDX(ARMY_ANIMATION_WALK)][0],
        facing == ARMY_FACING_LEFT ? ICON_DRAW_FLIPPED : ICON_DRAW_NORMAL,
        VIEW_ARMY_MONSTER_WIDGET_Z_ORDER,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (!monsterWidget)
        MemError();
    m_viewArmyWindow->AddWidget(monsterWidget, -1);
    gpResourceManager->Dispose(monsterIcon);

    strcpy(reinterpret_cast<char*>(armyName), gArmyNames[IDX(monsterType)]);
    armyName[0] = CyrillicToUpper(static_cast<char>(armyName[0]));
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = VIEW_ARMY_TITLE_WIDGET_ID;
    message.payload.widget.data.text = reinterpret_cast<char*>(armyName);
    m_viewArmyWindow->BroadcastMessage(message);

    details = static_cast<char*>(H2_ALLOC(VIEW_ARMY_DETAIL_BUFFER_SIZE));
    if (theGroup)
        morale = theGroup->GetMorale(theHero, castle, NULL);
    else
        morale = 0;
    if (HAS(monster->flags.all, MONSTER_FLAGS_NO_MORALE))
        morale = 0;

    sprintf(
        details,
        ""
    );
    modifier = 0;
    sprintf(
        gText,
        "%s%d",
        cArmyDetail[ARMY_DETAIL_ATTACK],
        static_cast<i32>(monster->attack)
    );
    strcat(details, gText);
    if (theHero)
        modifier += theHero->Stats(HERO_PRIMARY_ATTACK);
    if (theArmy)
        modifier = theArmy->m_monster.attack - monster->attack;
    if (modifier) {
        sprintf(gText, " (%d)", monster->attack + modifier);
        strcat(details, gText);
    }

    modifier = 0;
    sprintf(
        gText,
        "\n%s%d",
        cArmyDetail[ARMY_DETAIL_DEFENSE],
        static_cast<i32>(monster->defense)
    );
    strcat(details, gText);
    if (theHero)
        modifier += theHero->Stats(HERO_PRIMARY_DEFENSE);
    if (theArmy)
        modifier = theArmy->m_monster.defense - monster->defense;
    if (modifier) {
        sprintf(gText, " (%d)", monster->defense + modifier);
        strcat(details, gText);
    }

    if (HAS(monster->flags.all, MONSTER_FLAGS_SHOOTER)) {
        i32 remainingShots = armyMonster->shots;
        if (remainingShots > 0) {
            if (gpCombatManager->m_active == 1)
                sprintf(gText, "\n%s%d", cArmyDetail[ARMY_DETAIL_SHOTS_LEFT], remainingShots);
            else
                sprintf(gText, "\n%s%d", cArmyDetail[ARMY_DETAIL_SHOTS_OUTSIDE], remainingShots);
            strcat(details, gText);
        }
    }

    sprintf(
        gText,
        "\n%s%d",
        cArmyDetail[ARMY_DETAIL_DAMAGE],
        static_cast<i32>(monster->damageMin)
    );
    strcat(details, gText);
    if (monster->damageMin != monster->damageMax) {
        sprintf(gText, "-%d", static_cast<i32>(monster->damageMax));
        strcat(details, gText);
    }
    sprintf(
        gText,
        "\n%s%d",
        cArmyDetail[ARMY_DETAIL_HIT_POINTS],
        static_cast<u32>(monster->hitPoints)
    );
    strcat(details, gText);
    if (gpCombatManager->m_active == 1) {
        sprintf(
            gText,
            "\n%s%d",
            localization::Tr("army.hit_points_left.prefix"),
            static_cast<u32>(monster->hitPoints) - theArmy->m_hitPointsLost
        );
        strcat(details, gText);
    }
    sprintf(gText, "\n%s%s", cArmyDetail[ARMY_DETAIL_SPEED], speedText[armyMonster->speed]);
    strcat(details, gText);
    sprintf(
        gText,
        "\n%s%s",
        cArmyDetail[ARMY_DETAIL_MORALE],
        gMoraleText[morale + VIEW_ARMY_TEXT_NEUTRAL_OFFSET]
    );
    strcat(details, gText);
    luck = GetLuck(theHero, theArmy, castle);
    sprintf(
        gText,
        "\n%s%s",
        cArmyDetail[ARMY_DETAIL_LUCK],
        gLuckText[luck + VIEW_ARMY_TEXT_NEUTRAL_OFFSET]
    );
    strcat(details, gText);

    message.payload.widget.id = VIEW_ARMY_DETAIL_WIDGET_ID;
    message.payload.widget.data.text = details;
    m_viewArmyWindow->BroadcastMessage(message);
    if (!gbAllowUpgrade) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message.payload.widget.id = VIEW_ARMY_UPGRADE_ACTION_ID;
        m_viewArmyWindow->BroadcastMessage(message);
    }
    if (disableUpgrade) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message.payload.widget.id = VIEW_ARMY_UPGRADE_ID;
        m_viewArmyWindow->BroadcastMessage(message);
    }
    if (quickView) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message.payload.widget.id = VIEW_ARMY_QUICK_VIEW_ID;
        m_viewArmyWindow->BroadcastMessage(message);
    }
    if (numTroops < 1) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
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
        iconWidget* spellWidget;
        i32 spellY = VIEW_ARMY_SPELL_BASE_Y;
        i32 spellCenterX = VIEW_ARMY_SPELL_CENTER_X;
        if (quickView)
            spellY += VIEW_ARMY_QUICK_SPELL_Y_OFFSET;
        i32 spacing = VIEW_ARMY_SPELL_SPACING_BASE - theArmy->m_spellCount;
        i32 spellX =
            spellCenterX + (VIEW_ARMY_SPELL_X_BIAS - theArmy->m_spellCount)
            - (theArmy->m_spellCount * spacing) / VIEW_ARMY_ICON_CENTER_DIVISOR;
        H2_ENUM_STORAGE_STEPPED(ArmySpellInfluence, i32) spellIndex =
            ARMY_SPELL_INFLUENCE_NONE;
        for (loopIndex = 0;
             loopIndex < (theArmy->m_spellCount > VIEW_ARMY_SPELL_VISIBLE_LIMIT
                               ? VIEW_ARMY_SPELL_VISIBLE_LIMIT
                               : theArmy->m_spellCount);
             loopIndex++) {
            spellIndex++;
            for (; spellIndex < ARMY_SPELL_INFLUENCE_COUNT; spellIndex++) {
                if (theArmy->m_spellInfluence[IDX(spellIndex)])
                    break;
            }
            spellWidget = new iconWidget(
                static_cast<i16>(spellX + loopIndex * spacing),
                static_cast<i16>(spellY + VIEW_ARMY_SPELL_WIDGET_Y_OFFSET),
                0,
                0,
                const_cast<char*>("spellinl.icn"),
                static_cast<i16>(IDX(spellIndex)),
                ICON_DRAW_NORMAL,
                static_cast<i16>(loopIndex + VIEW_ARMY_SPELL_WIDGET_ID_BASE),
                WIDGET_KIND_ICON_DIRECT,
                1
            );
            if (!spellWidget)
                MemError();
            m_viewArmyWindow->AddWidget(spellWidget, -1);
        }
    }

    glTimers[0] = KBTickCount() + VIEW_ARMY_ANIMATION_INITIAL_DELAY;
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
    H2_FREE(details);
    delete m_viewArmyWindow;
}
#if H2_RETAIL_COMPILER
#undef armyMonster
#undef armyName
#undef baseX
#undef detailMessage
#undef details
#undef filename
#undef iconFrame
#undef luck
#undef modifier
#undef monsterIcon
#undef monsterWidget
#undef numWidget
#undef remainingShots
#undef spacing
#undef spellCenterX
#undef spellIndex
#undef spellWidget
#undef spellX
#undef spellY
#undef titleMessage
#endif

#if H2_RETAIL_COMPILER
#define frameDelay frameDelay6
#define resourceType resourceType7
#endif
VA(0x00456231, 0x37d)
MessageDispatchResult ViewArmyHandler(tag_message& message) {
    i32 resourceCost;
    i16 H2_UNUSED(frameDelay);
    i16 H2_UNUSED(frameOffset);
    i32 goldCost;
    ResourceType resourceType;

    gbDismissArmy = false;
    gbUpgradeArmy = false;
    frameDelay = VIEW_ARMY_HANDLER_FRAME_DELAY;

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case EVENT_WINDOW_FIRST_BUTTON:
                    case EVENT_WINDOW_SECOND_BUTTON:
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        message.payload.widget.id = VIEW_ARMY_CLOSE_ID;
                        message.payload.widget.command = BaseWidgetCommand(VIEW_ARMY_CLOSE_ID);
                        return MESSAGE_DISPATCH_FORWARD;
                    case EVENT_WINDOW_FOURTH_BUTTON:
                        NormalDialog(
                            const_cast<char*>(
                                localization::Tr("army.confirm.dismiss")
                            ),
                            NORMAL_DIALOG_CONFIRM
                        );
                        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
                            gbDismissArmy = true;
                            message.payload.widget.id = VIEW_ARMY_CLOSE_ID;
                            message.payload.widget.command = BaseWidgetCommand(VIEW_ARMY_CLOSE_ID);
                            return MESSAGE_DISPATCH_FORWARD;
                        }
                        break;
                    case VIEW_ARMY_UPGRADE_ACTION_ID:
                        goldCost = iViewArmyNumTroops * VIEW_ARMY_UPGRADE_COST_MULTIPLIER
                                   * (gMonsterDatabase[IDX(iViewArmyUpgradeToType)].cost
                                      - gMonsterDatabase[IDX(iViewArmyType)].cost);
                        if (iViewArmyUpgradeToType == CREATURE_BLACK_DRAGON) {
                            resourceType = RES_SULFUR;
                            resourceCost =
                                iViewArmyNumTroops * VIEW_ARMY_UPGRADE_COST_MULTIPLIER;
                        } else if (iViewArmyUpgradeToType == CREATURE_TITAN) {
                            resourceType = RES_GEMS;
                            resourceCost =
                                iViewArmyNumTroops * VIEW_ARMY_UPGRADE_COST_MULTIPLIER;
                        } else {
                            resourceType = RES_NONE;
                            resourceCost = 0;
                        }
                        if (gpCurPlayer->m_resources[IDX(RES_GOLD)] >= goldCost
                            && (resourceType == RES_NONE
                                || gpCurPlayer->m_resources[IDX(resourceType)] >= resourceCost)) {
                            NormalDialog(
                                const_cast<char*>(
                                    localization::Tr("army.upgrade.confirm")
                                ),
                                NORMAL_DIALOG_CONFIRM,
                                -1,
                                -1,
                                IDX(RES_GOLD),
                                goldCost,
                                IDX(resourceType),
                                resourceCost,
                                -1,
                                0
                            );
                            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
                                gpCurPlayer->m_resources[IDX(RES_GOLD)] -= goldCost;
                                if (resourceType != RES_NONE)
                                    gpCurPlayer->m_resources[IDX(resourceType)] -= resourceCost;
                                gbUpgradeArmy = true;
                                message.payload.widget.id = VIEW_ARMY_CLOSE_ID;
                                message.payload.widget.command =
                                    BaseWidgetCommand(VIEW_ARMY_CLOSE_ID);
                                return MESSAGE_DISPATCH_FORWARD;
                            }
                        } else {
                            NormalDialog(
                                const_cast<char*>(localization::Tr("army.upgrade.cannot_afford")),
                                NORMAL_DIALOG_INFO,
                                -1,
                                -1,
                                IDX(RES_GOLD),
                                goldCost,
                                IDX(resourceType),
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

    if (glTimers[0] < KBTickCount()) {
        SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_FRAME, VIEW_ARMY_MONSTER_WIDGET_ID);
        iViewArmyFrame = (iViewArmyFrame + 1)
                         % sViewArmyMonFrameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)];
        message.payload.widget.data.value =
            sViewArmyMonFrameInfo.animationFrames[IDX(ARMY_ANIMATION_WALK)][iViewArmyFrame];
        gpGame->m_viewArmyWindow->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_SET_X;
        message.payload.widget.data.value =
            viewArmyBaseX
            + viewArmyFacingWIPXMod * sViewArmyMonFrameInfo.walkXOffsets[iViewArmyFrame];
        gpGame->m_viewArmyWindow->BroadcastMessage(message);
        gpGame->m_viewArmyWindow->DrawWindow(1, 0, WINDOW_DRAW_ID_LIMIT);
        glTimers[0] = static_cast<i32>(
            KBTickCount()
            + sViewArmyMonFrameInfo.walkDuration * GAME_VIEW_ARMY_FRAME_DELAY_SCALE
                  / sViewArmyMonFrameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)]
        );
    }
    return MESSAGE_DISPATCH_CONSUME;
}
#if H2_RETAIL_COMPILER
#undef frameDelay
#undef resourceType
#endif

VA(0x004565ae, 0x646)
i32 game::GetRandomNumTroops(H2_ENUM_PARAM(CreatureType, i32) monsterType) {
    switch (monsterType) {
        case CREATURE_PEASANT:
            return Random(40, 80);
        case CREATURE_ARCHER:
            return Random(20, 30);
        case CREATURE_RANGER:
            return Random(20, 30);
        case CREATURE_PIKEMAN:
            return Random(20, 30);
        case CREATURE_VETERAN_PIKEMAN:
            return Random(20, 30);
        case CREATURE_SWORDSMAN:
            return Random(12, 25);
        case CREATURE_MASTER_SWORDSMAN:
            return Random(12, 25);
        case CREATURE_CAVALRY:
            return Random(10, 18);
        case CREATURE_CHAMPION:
            return Random(8, 16);
        case CREATURE_PALADIN:
            return Random(6, 12);
        case CREATURE_CRUSADER:
            return Random(6, 10);
        case CREATURE_GOBLIN:
            return Random(25, 40);
        case CREATURE_ORC:
            return Random(15, 30);
        case CREATURE_ORC_CHIEF:
            return Random(15, 30);
        case CREATURE_WOLF:
            return Random(20, 35);
        case CREATURE_OGRE:
            return Random(12, 25);
        case CREATURE_OGRE_LORD:
            return Random(10, 20);
        case CREATURE_TROLL:
            return Random(7, 10);
        case CREATURE_WAR_TROLL:
            return Random(7, 10);
        case CREATURE_CYCLOPS:
            return Random(5, 7);
        case CREATURE_SPRITE:
            return Random(25, 45);
        case CREATURE_DWARF:
            return Random(12, 25);
        case CREATURE_BATTLE_DWARF:
            return Random(10, 22);
        case CREATURE_ELF:
            return Random(15, 30);
        case CREATURE_GRAND_ELF:
            return Random(12, 28);
        case CREATURE_DRUID:
            return Random(10, 25);
        case CREATURE_GREATER_DRUID:
            return Random(10, 20);
        case CREATURE_UNICORN:
            return Random(8, 15);
        case CREATURE_PHOENIX:
            return Random(7, 12);
        case CREATURE_CENTAUR:
            return Random(20, 50);
        case CREATURE_GARGOYLE:
            return Random(15, 30);
        case CREATURE_GRIFFIN:
            return Random(12, 25);
        case CREATURE_MINOTAUR:
            return Random(10, 16);
        case CREATURE_MINOTAUR_KING:
            return Random(9, 16);
        case CREATURE_HYDRA:
            return Random(7, 10);
        case CREATURE_GREEN_DRAGON:
            return Random(4, 7);
        case CREATURE_RED_DRAGON:
            return Random(3, 7);
        case CREATURE_BLACK_DRAGON:
            return Random(3, 7);
        case CREATURE_HALFLING:
            return Random(20, 50);
        case CREATURE_BOAR:
            return Random(15, 30);
        case CREATURE_IRON_GOLEM:
            return Random(10, 25);
        case CREATURE_STEEL_GOLEM:
            return Random(10, 22);
        case CREATURE_ROC:
            return Random(10, 16);
        case CREATURE_MAGE:
            return Random(8, 12);
        case CREATURE_ARCHMAGE:
            return Random(7, 11);
        case CREATURE_GIANT:
            return Random(5, 8);
        case CREATURE_TITAN:
            return Random(3, 7);
        case CREATURE_SKELETON:
            return Random(20, 50);
        case CREATURE_ZOMBIE:
            return Random(15, 30);
        case CREATURE_MUTANT_ZOMBIE:
            return Random(15, 30);
        case CREATURE_MUMMY:
            return Random(10, 25);
        case CREATURE_ROYAL_MUMMY:
            return Random(10, 25);
        case CREATURE_VAMPIRE:
            return Random(8, 12);
        case CREATURE_VAMPIRE_LORD:
            return Random(8, 12);
        case CREATURE_LICH:
            return Random(6, 10);
        case CREATURE_POWER_LICH:
            return Random(6, 10);
        case CREATURE_BONE_DRAGON:
            return Random(4, 8);
        case CREATURE_ROGUE:
            return Random(20, 40);
        case CREATURE_NOMAD:
            return Random(12, 25);
        case CREATURE_GHOST:
            return Random(10, 20);
        case CREATURE_GENIE:
            return Random(5, 10);
        case CREATURE_MEDUSA:
            return Random(12, 20);
        case CREATURE_EARTH_ELEMENTAL:
            return Random(13, 25);
        case CREATURE_AIR_ELEMENTAL:
            return Random(13, 25);
        case CREATURE_FIRE_ELEMENTAL:
            return Random(13, 25);
        case CREATURE_WATER_ELEMENTAL:
            return Random(13, 25);
        default:
            return 3;
    }
}

VA(0x00456bf4, 0x2c)
void game::TurnOnAIMusic(void) {
    gpSoundManager->StopAllSamples(1);
    gpSoundManager->SwitchAmbientMusic(GAME_AI_MUSIC_TRACK);
    gSoundBackendsReady = 0;
}

VA(0x00456c20, 0x12)
void game::TurnOffAIMusic(void) {
    gSoundBackendsReady = 1;
}

VA(0x00456c32, 0x50a)
void game::NextPlayer(void) {
    i32 remotePlayer;
    i32 index;
    i32 H2_UNUSED(humansAlive);

    m_heroRecs[gpCurPlayer->m_availableHeroIds[0]].m_eventFlags = HeroEventFlag(
        static_cast<i32>(m_heroRecs[gpCurPlayer->m_availableHeroIds[0]].m_eventFlags)
        & ~IDX(HERO_EVENT_WEEKLY_VISIT)
    );
    m_heroRecs[gpCurPlayer->m_availableHeroIds[1]].m_eventFlags = HeroEventFlag(
        static_cast<i32>(m_heroRecs[gpCurPlayer->m_availableHeroIds[1]].m_eventFlags)
        & ~IDX(HERO_EVENT_WEEKLY_VISIT)
    );
    iCurHourGlassPhase = 0;

    if (gbThisNetHumanPlayer[giCurPlayer] && gConfig.autosave) {
        humansAlive = 0;
        for (index = 0; index < GAME_PLAYER_COUNT; index++) {
            if (m_playerDead[index] == 0 && gbHumanPlayer[index])
                humansAlive++;
        }
        SaveGame(
            const_cast<char*>(localization::Tr("save.filename.autosave")),
            1,
            0
        );
    }

    gpAdvManager->m_identifyHeroActive = false;
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
        SetNoDialogMenus(false);
        giBottomViewOverride = BOTTOM_VIEW_OVERRIDE_DISABLED;
        ShowComputerScreen();
        bShowIt = false;
        if (gbRemoteOn && gbHumanPlayer[giCurPlayer]) {
            gbThisNetGotAdventureControl = false;
            remotePlayer = gbGamePosToNetPos[giCurPlayer];
            if (!gpGame->TransmitSaveGame(remotePlayer, 0, 0))
                ShutDown(NULL);
        }
        if (giBottomViewOverride == BOTTOM_VIEW_OVERRIDE_DISABLED)
            giBottomViewOverride = BOTTOM_VIEW_NONE;
    } else {
        SetNoDialogMenus(true);
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
        giForceSwitchMusic = KBTickCount();
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
            giTerrainToMusicTrack[IDX(gpAdvManager->m_currentTerrain)]
        );
        gpAdvManager->SetEnvironmentOrigin(
            gpAdvManager->m_mapOriginX + ENVIRONMENT_ORIGIN_TILE_OFFSET,
            gpAdvManager->m_mapOriginY + ENVIRONMENT_ORIGIN_TILE_OFFSET,
            1
        );
    }
    if (gbThisNetHumanPlayer[giCurPlayer])
        gpAdvManager->ForceNewHover();
}

VA(0x0045713c, 0x38a)
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
            dailyGold += HAS(m_castleRecs[index].m_buildings, BIT(BUILDING_SLOT_UPGRADE_CASTLE))
                             ? DAILY_GOLD_VILLAGE_INCOME
                             : DAILY_GOLD_TOWN_INCOME;
            if (HAS(m_castleRecs[index].m_buildings, BIT(BUILDING_SLOT_SPECIAL_SEVEN)))
                dailyGold += DAILY_GOLD_STATUE_INCOME;
            if (m_castleRecs[index].m_type == FACTION_WARLOCK
                && HAS(m_castleRecs[index].m_buildings, BIT(BUILDING_SLOT_SPECIAL)))
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
            [IDX(gpGame->m_heroRecs[m_players[player].m_heroIds[heroIndex]]
                     .m_secondarySkills[IDX(HERO_SKILL_ESTATES)])];
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

#if H2_RETAIL_COMPILER
#define currentHero currentHero7
#define currentTown currentTown1
#define dailyIncome dailyIncome0
#define income income8
#define maxSpellPoints maxSpellPoints9
#define mineType resourceType1
#define resource resource8
#define restoredSpellPoints restoredSpellPoints14
#define townHero townHero6
#endif
VA(0x004574c6, 0xa10)
void game::PerDay(void) {
    i32 maxSpellPoints;
    i32 player;
    H2_ENUM_STORAGE_STEPPED(ResourceType, i32) resource;
    i32 H2_UNUSED(income);
    i32 dailyIncome;
    MineType mineType;
    hero* currentHero;
    i32 restoredSpellPoints;
    hero* townHero;
    town* currentTown;

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource = RES_WOOD; resource < RES_COUNT; resource++) {
            gpGame->m_players[player].m_aiData.m_income[IDX(resource)] =
                -m_players[player].m_resources[IDX(resource)];
        }
    }

    memset(m_dailyEventFlags, 0, sizeof(m_dailyEventFlags));

    for (player = 0; player < GAME_MINE_COUNT; player++) {
        if (m_mines[player].owner != -1) {
            mineType = m_mines[player].resourceType;
            dailyIncome = 0;
            if (mineType == MINE_TYPE_ORE)
                dailyIncome = DAILY_MINE_BULK_RESOURCE_INCOME;
            else if (mineType == MINE_TYPE_WOOD)
                dailyIncome = DAILY_MINE_BULK_RESOURCE_INCOME;
            else if (mineType != MINE_TYPE_GOLD)
                dailyIncome = 1;

            if (mineType != MINE_TYPE_GOLD && mineType <= MINE_TYPE_GOLD)
                m_players[m_mines[player].owner].m_resources[IDX(mineType)] += dailyIncome;
        }
    }

    for (player = 0; player < GAME_TOWN_COUNT; player++)
        m_castleRecs[player].m_turnsOwned++;

    for (player = 0; player < m_playerCount; player++) {
        m_players[player].m_resources[IDX(RES_SULFUR)] +=
            m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_POUCH_SULFUR);
        m_players[player].m_resources[IDX(RES_MERCURY)] +=
            m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_VIAL_MERCURY);
        m_players[player].m_resources[IDX(RES_GEMS)] +=
            m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_POUCH_GEMS);
        m_players[player].m_resources[IDX(RES_WOOD)] +=
            m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_CORD_WOOD);
        m_players[player].m_resources[IDX(RES_ORE)] +=
            m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_CART_ORE);
        m_players[player].m_resources[IDX(RES_CRYSTAL)] +=
            m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_POUCH_CRYSTAL);
        m_players[player].m_resources[IDX(RES_GOLD)] += ComputeDailyGold(player);
    }

    if (xIsPlayingExpansionCampaign && xCampaign.HasAward(AWARD_WOOD_BONUS))
        m_players[0].m_resources[IDX(RES_WOOD)] += DAILY_CAMPAIGN_WOOD_BONUS;

    for (player = 0; player < m_playerCount; player++) {
        if (!gbHumanPlayer[player]) {
            if (gpGame->m_difficulty >= DIFFICULTY_HARD) {
                m_players[player].m_resources[IDX(RES_WOOD)]++;
                m_players[player].m_resources[IDX(RES_ORE)]++;
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
    giCurTurn = GAME_DAY_NUMBER(*this);
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
            static_cast<i32>(m_heroRecs[player].m_eventFlags) & ~IDX(WEEKLY_HERO_RESERVED_FLAG)
        );

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource = RES_WOOD; resource < RES_GOLD; resource++) {
            if (m_playerHandicap[player] != PLAYER_HANDICAP_NONE) {
                m_players[player].m_resources[IDX(resource)] -= static_cast<i32>(
                    (gpGame->m_players[player].m_aiData.m_income[IDX(resource)]
                     + m_players[player].m_resources[IDX(resource)])
                    * (m_playerHandicap[player] == PLAYER_HANDICAP_MODERATE ? GAME_HANDICAP_MODERATE_DAILY_PENALTY : GAME_HANDICAP_SEVERE_DAILY_PENALTY)
                );
            }
        }
    }

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource = RES_WOOD; resource < RES_COUNT; resource++) {
            gpGame->m_players[player].m_aiData.m_income[IDX(resource)] +=
                m_players[player].m_resources[IDX(resource)];
        }
    }

    for (player = 0; player < GAME_HERO_COUNT; player++) {
        currentHero = &m_heroRecs[player];
        restoredSpellPoints = currentHero->m_spellPoints;
        maxSpellPoints = HERO_NORMAL_SPELL_POINTS(*currentHero);
        restoredSpellPoints +=
            IDX(currentHero->m_secondarySkills[IDX(HERO_SKILL_MYSTICISM)]) + 1;
        if (currentHero->HasArtifact(ARTIFACT_POWER_RING))
            restoredSpellPoints += POWER_RING_DAILY_MANA_BONUS;
        if (restoredSpellPoints > maxSpellPoints)
            restoredSpellPoints = maxSpellPoints;
        if (restoredSpellPoints > currentHero->m_spellPoints)
            currentHero->m_spellPoints = static_cast<i16>(restoredSpellPoints);
        if (HAS(currentHero->m_eventFlags, HERO_EVENT_MAGIC_WELL))
            H2_ENUM_CLEAR_FLAG(currentHero->m_eventFlags, HERO_EVENT_MAGIC_WELL);
    }

    for (player = 0; player < GAME_TOWN_COUNT; player++) {
        currentTown = GetTown(player);
        if (!HAS(currentTown->m_buildings, BIT(BUILDING_SLOT_MAGE_GUILD)))
            continue;
        if (currentTown->m_occupyingHeroId != -1) {
            townHero = GetHero(currentTown->m_occupyingHeroId);
            maxSpellPoints = HERO_NORMAL_SPELL_POINTS(*townHero);
            if (maxSpellPoints > townHero->m_spellPoints)
                townHero->m_spellPoints = static_cast<i16>(maxSpellPoints);
        }
    }
}
#if H2_RETAIL_COMPILER
#undef currentHero
#undef currentTown
#undef dailyIncome
#undef income
#undef maxSpellPoints
#undef mineType
#undef resource
#undef restoredSpellPoints
#undef townHero
#endif

#if H2_RETAIL_COMPILER
#define castle castle5
#define creatureGrowth growth2
#define mapX mapX10
#define mapY mapY7
#define monsterIncrease monsterIncrease8
#endif
VA(0x00457ed6, 0x1aa8)
void game::PerWeek(void) {
    FactionType heroClass = FACTION_KNIGHT;
    i32 mapY;
    i32 mapX;
    FactionType desiredClass;
    i32 outerIndex;
    i32 innerIndex;
    i32 monsterCount;
    i32 creatureGrowth;
    town* castle;
    hero* weeklyHero;
    i32 monsterIncrease;

    giWeekType = CALENDAR_PERIOD_NORMAL;
    giWeekTypeExtra = Random(0, WEEK_NAME_LAST);
    if (m_week != GAME_WEEKS_PER_MONTH) {
        outerIndex = Random(1, SPECIAL_WEEK_ROLL_MAX);
        if (outerIndex == 1) {
            giWeekType = CALENDAR_PERIOD_CREATURE;
            giWeekTypeExtra = Random(0, CREATURE_WEEK_LAST);
        }
    }

    for (outerIndex = 0; outerIndex < GAME_TOWN_COUNT; outerIndex++) {
        castle = GetTown(outerIndex);
        for (innerIndex = WEEKLY_FIRST_DWELLING; innerIndex <= WEEKLY_LAST_DWELLING;
             innerIndex++) {
            if (HAS(castle->m_buildings, (1 << innerIndex))) {
                creatureGrowth = gMonsterDatabase[IDX(gDwellingType[IDX(castle->m_type)]
                                                             [innerIndex - WEEKLY_FIRST_DWELLING])]
                               .growth;
                if (HAS(castle->m_buildings, BIT(BUILDING_SLOT_SPECIAL_FOUR)))
                    creatureGrowth += CASTLE_GROWTH_SPECIAL_BONUS;
                if (innerIndex == WEEKLY_FIRST_DWELLING
                    && HAS(castle->m_buildings, BIT(BUILDING_SLOT_WELL_EXTRA)))
                    creatureGrowth += CASTLE_GROWTH_WELL_BONUS;
                if (castle->m_owner == -1)
                    creatureGrowth /= NEUTRAL_CASTLE_GROWTH_DIVISOR;
                if (castle->m_owner >= 0
                    && castle->m_garrison[innerIndex - WEEKLY_FIRST_DWELLING] == 0
                    && !gbHumanPlayer[IDX(castle->m_owner)]) {
                    if (gpGame->m_difficulty == DIFFICULTY_HARD)
                        creatureGrowth = static_cast<i32>(creatureGrowth * WEEKLY_HARD_GROWTH_FACTOR);
                    if (gpGame->m_difficulty == DIFFICULTY_EXPERT)
                        creatureGrowth = static_cast<i32>(creatureGrowth * WEEKLY_EXPERT_GROWTH_FACTOR);
                    if (gpGame->m_difficulty == DIFFICULTY_IMPOSSIBLE)
                        creatureGrowth = static_cast<i32>(creatureGrowth * WEEKLY_IMPOSSIBLE_GROWTH_FACTOR);
                }
                if (giWeekType == CALENDAR_PERIOD_CREATURE
                    && IDX(gDwellingType[IDX(castle->m_type)]
                                        [innerIndex - WEEKLY_FIRST_DWELLING])
                           == giWeekTypeExtra)
                    creatureGrowth += CREATURE_WEEK_GROWTH_BONUS;
                castle->m_garrison[innerIndex - WEEKLY_FIRST_DWELLING] += creatureGrowth;
            }
        }
    }

    for (outerIndex = 0; outerIndex < GAME_PLAYER_COUNT; outerIndex++) {
        for (innerIndex = 0; innerIndex < AVAILABLE_HERO_SLOTS; innerIndex++) {
            if (innerIndex == 1) {
                heroClass =
                    m_heroRecs[gpGame->m_players[outerIndex].m_availableHeroIds[0]].m_cursorType;
            }
            heroClass = static_cast<FactionType>(
                (Random(1, IDX(FACTION_COUNT) - 1) + IDX(heroClass)) % IDX(FACTION_COUNT)
            );
            desiredClass = heroClass;
            if (innerIndex == 0
                && m_setupPlayerRace[gcColorToSetupPos[m_players[outerIndex].m_color]]
                       < FACTION_COUNT) {
                desiredClass =
                    m_setupPlayerRace[gcColorToSetupPos[m_players[outerIndex].m_color]];
            }

            if (gpGame->m_availableHeroes[gpGame->m_players[outerIndex].m_availableHeroIds[innerIndex]]
                == WEEKLY_AVAILABLE_HERO) {
                if (HAS(gpGame
                            ->m_heroRecs[gpGame->m_players[outerIndex].m_availableHeroIds[innerIndex]]
                            .m_eventFlags,
                        WEEKLY_HERO_RESERVED_FLAG))
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

    for (mapY = 0; mapY < MAP_HEIGHT; mapY++) {
        for (mapX = 0; mapX < MAP_WIDTH; mapX++) {
            switch (WORLDMAP->GetCell(mapX, mapY)->m_triggerType) {
                case MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER): {
                    monsterCount = WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata
                                     & IDX(MAP_MONSTER_COUNT_MASK);
                    monsterIncrease = monsterCount / EVENT_DAYS_PER_WEEK;
                    if (Random(1, EVENT_DAYS_PER_WEEK)
                        <= static_cast<i32>(monsterCount % EVENT_DAYS_PER_WEEK))
                        monsterIncrease++;
                    monsterCount += monsterIncrease;
                    if (monsterCount > WEEKLY_MONSTER_LIMIT)
                        monsterCount = WEEKLY_MONSTER_LIMIT;
                    WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata =
                        (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata
                         & IDX(MAP_MONSTER_GUARD_FLAG))
                        | monsterCount;
                    break;
                }
                case MAP_ACTION_TRIGGER(MAP_OBJECT_ARTESIAN_SPRING):
                    WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata = 1;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_WATER_WHEEL):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata
                        != WEEKLY_WATER_WHEEL_EMPTY)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata = 2;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_MAGIC_GARDEN):
                    WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata = Random(0, 1) ? 7 : 6;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_WINDMILL):
                    WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata = Random(1, 5);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_ARCHER_HOUSE):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(2, 4);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_GOBLIN_HUT):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_DWARF_COTTAGE):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(2, 4);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_PEASANT_HUT):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(5, 10);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_LOG_CABIN):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(5, 10);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_DESERT_TENT):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_WAGON_CAMP):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_TREE_HOUSE):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(4, 8);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_SIRENS):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_WATCH_TOWER):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(1, 4);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RUINS):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_TREE_CITY):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata
                        < WEEKLY_MONSTER_POPULATION_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(10, 20);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_CAVE):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_EXCAVATION):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(4, 8);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_HALFLING_HOLE):
                    if (WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(5, 10);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_TROLL_BRIDGE):
                    if (!(WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata
                          & WEEKLY_DWELLING_NO_GROWTH_FLAG)
                        && WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata
                               < WEEKLY_DRAGON_CITY_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_CITY_OF_DEAD):
                    if (!(WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata
                          & WEEKLY_DWELLING_NO_GROWTH_FLAG)
                        && WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata
                               < WEEKLY_DRAGON_CITY_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_DRAGON_CITY):
                    if (!(WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata
                          & WEEKLY_DWELLING_NO_GROWTH_FLAG)
                        && WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata
                               < WEEKLY_DRAGON_CITY_LIMIT)
                        WORLDMAP->GetCell(mapX, mapY)->m_objectMetadata += 1;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_EXPANSION_DWELLING):
                    WeeklyRecruitSite(WORLDMAP->GetCell(mapX, mapY));
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_EXPANSION_OBJECT):
                    WeeklyGenericSite(WORLDMAP->GetCell(mapX, mapY));
                    break;
                default:
                    break;
            }
        }
    }

    for (outerIndex = 0; outerIndex < GAME_HERO_COUNT; outerIndex++) {
        weeklyHero = &m_heroRecs[outerIndex];
        if (HAS(weeklyHero->m_eventFlags, WEEKLY_HERO_VISIT_FLAG))
            H2_ENUM_CLEAR_FLAG(weeklyHero->m_eventFlags, WEEKLY_HERO_VISIT_FLAG);
    }

    m_week++;
    SetupNewRumour();
    GiveTroopsToNeutralTowns();
}
#if H2_RETAIL_COMPILER
#undef castle
#undef creatureGrowth
#undef mapX
#undef mapY
#undef monsterIncrease
#endif

VA(0x0045997e, 0x13a)
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

VA(0x00459ab8, 0x61)
void game::WeeklyGenericSite(mapCell* cell) {
    i32 type = cell->m_objectMetadata;
    type &= WEEKLY_SITE_TYPE_MASK;
    switch (type) {
        case 4:
            cell->m_objectMetadata = type;
            break;
    }
}

#if H2_RETAIL_COMPILER
#define townPointer twn
#endif
VA(0x00459b19, 0x383)
void game::PerMonth(void) {
    mapCell* spot;
    i32 x;
    i32 i;
    i32 y;
    i32 growth;
    town* townPointer;
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
            townPointer = GetTown(i);
            if (HAS(townPointer->m_buildings, (1 << j))) {
                growth = gMonsterDatabase[IDX(gDwellingType[IDX(townPointer->m_type)]
                                                           [j - WEEKLY_FIRST_DWELLING])]
                              .growth;
                if (HAS(townPointer->m_buildings, WELL_BUILDING))
                    growth += WELL_GROWTH;
                if (j == WEEKLY_FIRST_DWELLING
                    && HAS(townPointer->m_buildings, FIRST_DWELLING_BONUS_BUILDING))
                    growth += FIRST_DWELLING_GROWTH;

                if (giMonthType == CALENDAR_PERIOD_CREATURE
                    && IDX(gDwellingType[IDX(townPointer->m_type)][j - WEEKLY_FIRST_DWELLING])
                           == giMonthTypeExtra)
                    townPointer->m_garrison[j - WEEKLY_FIRST_DWELLING] *= CREATURE_MONTH_MULTIPLIER;

                if (giMonthType == CALENDAR_PERIOD_PLAGUE) {
                    townPointer->m_garrison[j - WEEKLY_FIRST_DWELLING] -= growth;
                    if (townPointer->m_garrison[j - WEEKLY_FIRST_DWELLING] < 0)
                        townPointer->m_garrison[j - WEEKLY_FIRST_DWELLING] = 0;
                    townPointer->m_garrison[j - WEEKLY_FIRST_DWELLING] =
                        townPointer->m_garrison[j - WEEKLY_FIRST_DWELLING] >> 1;
                }
            }
        }
    }

    if (giMonthType == CALENDAR_PERIOD_CREATURE) {
        for (x = 0; x < MAP_WIDTH; x++) {
            for (y = 0; y < MAP_HEIGHT; y++) {
                spot = gpAdvManager->GetCell(x, y);
                if (spot->m_triggerType == MAP_OBJECT_NONE && !spot->m_objectLayerBit1
                    && !spot->m_objectLayerBit0 && CELL_TERRAIN(spot) != TERRAIN_WATER) {
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
                            spot->m_objectMetadata |= IDX(MAP_MONSTER_GUARD_FLAG);
                    }
                }
            }
        }
    }

    gpAdvManager->CompleteDraw(0);
}
#if H2_RETAIL_COMPILER
#undef townPointer
#endif

#if H2_RETAIL_COMPILER
#define extra ext
#endif
VA(0x00459e9c, 0x3c2)
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
    H2_ENUM_PARAM(MapObjectType, i32) oldTrigger,
    H2_ENUM_PARAM(MapObjectType, i32) newTrigger
) {
    i32 x;
    i32 y;
    mapCell* cell;
    mapCellExtra* extra;

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
                cell->m_triggerType = MAP_TRIGGER_WITH_OBJECT(cell->m_triggerType, newTrigger);

            if (cell->m_extraIndex != 0
                && WORLDMAP->Extra(cell->m_extraIndex)->objectIndex != static_cast<u8>(-1))
                extra = WORLDMAP->Extra(cell->m_extraIndex);
            else
                extra = NULL;
            while (extra != NULL) {
                if (extra->objectTileset == oldTileset
                    && extra->objectIndex >= oldFirstIndex
                    && extra->objectIndex <= oldLastIndex) {
                    extra->objectTileset = newTileset;
                    extra->objectIndex =
                        static_cast<u8>(extra->objectIndex - oldFirstIndex + newFirstIndex);
                }
                if (extra->nextIndex != 0
                    && WORLDMAP->Extra(extra->nextIndex)->objectIndex != static_cast<u8>(-1))
                    extra = WORLDMAP->Extra(extra->nextIndex);
                else
                    extra = NULL;
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
                extra = WORLDMAP->Extra(cell->m_extraIndex);
            else
                extra = NULL;
            while (extra != NULL) {
                if (extra->overlayTileset == oldTileset
                    && extra->overlayIndex >= oldFirstIndex
                    && extra->overlayIndex <= oldLastIndex) {
                    extra->overlayTileset = newTileset;
                    extra->overlayIndex =
                        static_cast<u8>(extra->overlayIndex - oldFirstIndex + newFirstIndex);
                }
                if (extra->nextIndex != 0
                    && WORLDMAP->Extra(extra->nextIndex)->overlayIndex != static_cast<u8>(-1))
                    extra = WORLDMAP->Extra(extra->nextIndex);
                else
                    extra = NULL;
            }
        }
    }
}
#if H2_RETAIL_COMPILER
#undef extra
#endif

VA(0x0045a25e, 0x1b0)
void game::RandomizeTown(i32 x, i32 y, i32) {
    i32 H2_UNUSED(unused)[RANDOM_TOWN_SCRATCH_WIDTH];
    i32 townId = GetTownId(x, y);
    town* castle = GetTown(townId);
    mapTownExtra* townExtra =
        reinterpret_cast<mapTownExtra*>(ppMapExtra[WORLDMAP->GetCell(x, y)->m_objectMetadata]);
    FactionType race;

    if (townExtra->color == RANDOM_TOWN_UNOWNED_COLOR)
        race = static_cast<FactionType>(Random(RANDOM_TOWN_RACE_MIN, RANDOM_TOWN_RACE_MAX));
    else
        race = m_setupPlayerRace[gcColorToSetupPos[townExtra->color]];

    castle->m_turnsOwned = RANDOM_TOWN_AGE;
    ConvertObject(
        x + RANDOM_TOWN_LEFT,
        y + RANDOM_TOWN_TOP,
        x + RANDOM_TOWN_RIGHT,
        y + RANDOM_TOWN_BOTTOM,
        RANDOM_TOWN_SOURCE_TILESET,
        RANDOM_TOWN_OBJECT_SOURCE_FIRST,
        RANDOM_TOWN_OBJECT_SOURCE_LAST,
        RANDOM_TOWN_OBJECT_TILESET,
        IDX(race) << RANDOM_TOWN_RACE_FRAME_SHIFT,
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
        IDX(race) << RANDOM_TOWN_RACE_FRAME_SHIFT,
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
        IDX(race) << RANDOM_TOWN_RACE_FRAME_SHIFT,
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
        IDX(race) << RANDOM_TOWN_RACE_FRAME_SHIFT,
        MAP_OBJECT_RANDOM_CASTLE,
        MAP_OBJECT_CASTLE
    );
    m_castleRecs[townId].m_type = race;
}

#if H2_RETAIL_COMPILER
#define mineIndex mineIdx
#endif
VA(0x0045a40e, 0x522)
void game::RandomizeMine(i32 x, i32 y) {
    u8 objFrame;
    i32 mineIndex;
    i32 iRow;
    i32 count;
    i32 iCol;
    MapObjectType trigger;
    MineType resType;
    H2_ENUM_STORAGE(TerrainType, i32) terrain;
    u8 mineFrame;

    terrain = CELL_TERRAIN(WORLDMAP->GetCell(x, y));
    for (count = 0; count < RANDOM_MINE_RETRY_LIMIT; count++) {
        switch (terrain) {
            case TERRAIN_GRASS:
            case TERRAIN_DIRT:
                resType = static_cast<MineType>(Random(IDX(MINE_TYPE_MERCURY), IDX(MINE_TYPE_GOLD)));
                if (resType == MINE_TYPE_MERCURY)
                    resType = MINE_TYPE_WOOD;
                break;
            case TERRAIN_SNOW:
                resType = static_cast<MineType>(Random(IDX(MINE_TYPE_ORE), IDX(MINE_TYPE_GOLD)));
                break;
            case TERRAIN_SWAMP:
                resType = static_cast<MineType>(Random(IDX(MINE_TYPE_WOOD), IDX(MINE_TYPE_GOLD)));
                break;
            case TERRAIN_LAVA:
                resType = MINE_TYPE_MERCURY;
                break;
            default:
                resType = static_cast<MineType>(Random(IDX(MINE_TYPE_MERCURY), IDX(MINE_TYPE_GOLD)));
                break;
        }
        if (RandMineQty[IDX(resType)] == 0)
            count = RANDOM_MINE_RETRY_LIMIT;
    }
    RandMineQty[IDX(resType)]++;

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
            IDX(resType) - IDX(MINE_TYPE_ORE),
            0,
            -1
        );
        trigger = MAP_OBJECT_MINE;
    }

    mineIndex = GetMineId(x, y);
    for (iRow = 0; iRow < RANDOM_MINE_FOOTPRINT_WIDTH; iRow++) {
        for (iCol = 0; iCol < RANDOM_MINE_FOOTPRINT_WIDTH; iCol++) {
            if ((WORLDMAP->GetCell(x + iCol, y - iRow)->m_triggerType
                 & MAP_TRIGGER_TYPE_MASK)
                    > MAP_OBJECT_NONE
                && IDX(
                       WORLDMAP->GetCell(x + iCol, y - iRow)->m_triggerType
                       & MAP_TRIGGER_TYPE_MASK
                   )
                       <= MINE_FLAG_OVERWRITE_LIMIT) {
                // a lower-numbered trigger already owns this tile
            } else {
                WORLDMAP->GetCell(x + iCol, y - iRow)->m_objectMetadata = mineIndex;
                WORLDMAP->GetCell(x + iCol, y - iRow)->m_triggerType = trigger;
            }
        }
    }
    WORLDMAP->GetCell(x, y)->m_triggerType |= MAP_TRIGGER_ACTION_FLAG;
    m_mines[mineIndex].resourceType = resType;
}
#if H2_RETAIL_COMPILER
#undef mineIndex
#endif

#if H2_RETAIL_COMPILER
#define unusedValue unused4
#endif
VA(0x0045a930, 0xb8)
void game::InitRandomArtifacts(void) {
    i32 H2_UNUSED(unusedValue);
    i32 y;
    memset(m_randomArtifacts, 0, sizeof(m_randomArtifacts));
    for (i32 x = 0; x < MAP_WIDTH; x++) {
        for (y = 0; y < MAP_HEIGHT; y++) {
            mapCell* cell = WORLDMAP->GetCell(x, y);
            if (cell->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_ARTIFACT)))
                m_randomArtifacts[cell->m_objectIndex / 2] = 1;
        }
    }
}
#if H2_RETAIL_COMPILER
#undef unusedValue
#endif

VA(0x0045a9e8, 0x132)
i32 game::GetRandomArtifactId(
    H2_ENUM_PARAM(ArtifactLevelMask, i32) levelMask,
    b32 allowCursed
) {
    i32 tries = 0;
    ArtifactType artifact;

    for (;;) {
        if (xIsExpansionMap)
            artifact = static_cast<ArtifactType>(Random(ARTIFACT_FIRST, ARTIFACT_EXPANSION_LAST));
        else
            artifact = static_cast<ArtifactType>(Random(ARTIFACT_FIRST, ARTIFACT_BASE_LAST));

        if (!HAS(gArtifactLevel[IDX(artifact)], levelMask))
            continue;
        if (artifact == ARTIFACT_EDITOR_ANY_ULTIMATE || artifact == ARTIFACT_EDITOR_UNUSED_84
            || artifact == ARTIFACT_EDITOR_UNUSED_85 || artifact == ARTIFACT_EDITOR_UNUSED_86
            || artifact == ARTIFACT_SPELL_SCROLL)
            continue;
        if (xIsPlayingExpansionCampaign) {
            if (artifact == ARTIFACT_BREASTPLATE_ANDURAN || artifact == ARTIFACT_BATTLE_GARB
                || artifact == ARTIFACT_HELMET_ANDURAN || artifact == ARTIFACT_SWORD_ANDURAN
                || artifact == ARTIFACT_SPHERE_NEGATION)
                continue;
        }
        if (tries++ < ARTIFACT_UNIQUE_RETRIES && m_randomArtifacts[IDX(artifact)])
            continue;
        if (IsCursedItem(artifact)) {
            if (!allowCursed)
                continue;
            if (Random(ARTIFACT_FIRST, ARTIFACT_CURSED_ROLL_MAX)
                < ARTIFACT_CURSED_REJECT_CHANCE)
                continue;
        }
        if (m_mapHeader.victoryCondition != MAP_VICTORY_FIND_ARTIFACT
            || m_mapHeader.victoryConditionValue - ARTIFACT_ID_OFFSET != IDX(artifact))
            break;
    }

    m_randomArtifacts[IDX(artifact)] = 1;
    return IDX(artifact);
}

VA(0x0045ab1a, 0x3e)
i32 IsCursedItem(ArtifactType item) {
    if (item == ARTIFACT_FIZBIN_OF_MISFORTUNE || item == ARTIFACT_HIDEOUS_MASK
        || item == ARTIFACT_TAX_LIEN || item == ARTIFACT_ARM_OF_MARTYR
        || item == ARTIFACT_BROACH_SHIELDING || item == ARTIFACT_HEART_FIRE
        || item == ARTIFACT_HEART_ICE)
        return 1;
    return 0;
}

VA(0x0045ab58, 0x1a8)
void game::RandomizeHeroPool(void) {
    for (i32 heroId = 0; heroId < RANDOM_HERO_COUNT; heroId++) {
        m_heroRecs[heroId].m_experience =
            Random(RANDOM_HERO_EXPERIENCE_MIN, RANDOM_HERO_EXPERIENCE_MAX)
            + RANDOM_HERO_EXPERIENCE_BASE;
        SetRandomHeroArmies(heroId, RANDOM_HERO_NORMAL_ARMY);
        m_heroRecs[heroId].m_remainingMobility = m_heroRecs[heroId].CalcMobility();
        m_heroRecs[heroId].m_mobility = m_heroRecs[heroId].m_remainingMobility;
        m_heroRecs[heroId].m_randomSeed =
            static_cast<u8>(Random(RANDOM_HERO_SEED_MIN, RANDOM_HERO_SEED_MAX));
        m_heroRecs[heroId].m_enabled = RANDOM_HERO_ENABLED;

        if (m_heroRecs[heroId].m_cursorType == FACTION_SORCERESS)
            m_heroRecs[heroId].m_spells[IDX(SPELL_BLESS)] = RANDOM_HERO_STARTING_SPELL_KNOWN;
        else if (m_heroRecs[heroId].m_cursorType == FACTION_WARLOCK)
            m_heroRecs[heroId].m_spells[IDX(SPELL_CURSE)] = RANDOM_HERO_STARTING_SPELL_KNOWN;
        else if (m_heroRecs[heroId].m_cursorType == FACTION_NECROMANCER)
            m_heroRecs[heroId].m_spells[IDX(SPELL_HASTE)] = RANDOM_HERO_STARTING_SPELL_KNOWN;
        else if (m_heroRecs[heroId].m_cursorType == FACTION_WIZARD)
            m_heroRecs[heroId].m_spells[IDX(SPELL_STONE_SKIN)] = RANDOM_HERO_STARTING_SPELL_KNOWN;
    }
}

#if H2_RETAIL_COMPILER
#define army army2
#define armySlot armySlot16
#define minimumCount minimum3
#endif
VA(0x0045ad00, 0x342)
void game::SetRandomHeroArmies(i32 heroId, i32 strongArmy) {
    armyGroup* army = &m_heroRecs[heroId].m_army;
    i32 armySlot = 0;
    RandomHeroArmyRange armyTable[IDX(FACTION_COUNT)][RANDOM_HERO_ARMY_OPTION_COUNT] = {
        {{IDX(CREATURE_PEASANT), 30, 50},
         {IDX(CREATURE_ARCHER), 3, 5},
         {IDX(CREATURE_PIKEMAN), 2, 4}},
        {{IDX(CREATURE_GOBLIN), 15, 25}, {IDX(CREATURE_ORC), 3, 5}, {IDX(CREATURE_WOLF), 2, 3}},
        {{IDX(CREATURE_SPRITE), 10, 20}, {IDX(CREATURE_DWARF), 2, 4}, {IDX(CREATURE_ELF), 1, 2}},
        {{IDX(CREATURE_CENTAUR), 6, 10},
         {IDX(CREATURE_GARGOYLE), 2, 4},
         {IDX(CREATURE_GRIFFIN), 1, 2}},
        {{IDX(CREATURE_HALFLING), 6, 10},
         {IDX(CREATURE_BOAR), 2, 4},
         {IDX(CREATURE_IRON_GOLEM), 1, 2}},
        {{IDX(CREATURE_SKELETON), 6, 10}, {IDX(CREATURE_ZOMBIE), 2, 4}, {IDX(CREATURE_MUMMY), 1, 2}}
    };
    b32 selected[RANDOM_HERO_ARMY_OPTION_COUNT];
    i32 index;
    i32 minimumCount;
    i32 maximum;

    selected[0] = true;
    selected[1] = Random(RANDOM_HERO_PERCENT_MIN, RANDOM_HERO_PERCENT_MAX)
                  < RANDOM_HERO_FIRST_STACK_CHANCE
                        + (strongArmy ? RANDOM_HERO_FIRST_STACK_BONUS_CHANCE : 0);
    selected[RANDOM_HERO_SECOND_SELECTION] =
        Random(RANDOM_HERO_PERCENT_MIN, RANDOM_HERO_PERCENT_MAX)
        < RANDOM_HERO_SECOND_STACK_CHANCE
              + (strongArmy ? RANDOM_HERO_SECOND_STACK_BONUS_CHANCE : 0);
    if (!selected[RANDOM_HERO_SECOND_SELECTION])
        selected[1] = true;

    for (index = 0; index < RANDOM_HERO_ARMY_SLOT_COUNT; index++) {
        army->m_creatureTypes[index] = CREATURE_NONE;
        army->m_creatureCounts[index] = RANDOM_HERO_EMPTY_COUNT;
    }

    for (index = 0; index < RANDOM_HERO_ARMY_SELECTION_COUNT; index++) {
        if (selected[index]) {
            army->m_creatureTypes[armySlot] =
                static_cast<i8>(armyTable[IDX(m_heroRecs[heroId].m_cursorType)][index].creature);
            minimumCount = armyTable[IDX(m_heroRecs[heroId].m_cursorType)][index].minimum
                       * RANDOM_HERO_COUNT_SCALE;
            maximum = armyTable[IDX(m_heroRecs[heroId].m_cursorType)][index].maximum
                           * RANDOM_HERO_COUNT_SCALE
                       + RANDOM_HERO_COUNT_ROUNDING;
            if (strongArmy)
                minimumCount =
                    (minimumCount + maximum) / RANDOM_HERO_AVERAGE_DIVISOR;
            army->m_creatureCounts[armySlot] =
                static_cast<i16>(Random(minimumCount, maximum) / RANDOM_HERO_COUNT_SCALE);
            armySlot++;
        }
    }
}
#if H2_RETAIL_COMPILER
#undef army
#undef armySlot
#undef minimumCount
#endif

#if H2_RETAIL_COMPILER
#define mineIndex mineIndex2
#define randomObjectType randomObjectType8
#define randomType randomType8
#endif
VA(0x0045b042, 0x6f4)
void game::ProcessRandomObjects(void) {
    i32 artifactId;
    i32 minValue;
    i32 mineIndex;
    i32 x;
    i32 y;
    i32 maxValue;
    mapCell* cell;
    H2_OPEN_CODE_PARAM(MapTriggerCode, i32) randomObjectType;
    i32 randomType;

    giUABaseX = -1;
    giUABaseY = -1;
    giUARadius = 0;
    for (mineIndex = 0; mineIndex < RANDOM_MINE_RESOURCE_COUNT; mineIndex++)
        RandMineQty[mineIndex] = 0;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            cell = WORLDMAP->GetCell(x, y);
            switch (cell->m_triggerType) {
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT):
                    giUABaseX = static_cast<i16>(x);
                    giUABaseY = static_cast<i16>(y);
                    giUARadius = static_cast<i16>(cell->m_objectMetadata);
                    cell->m_triggerType = MAP_OBJECT_NONE;
                    cell->m_objectTileset = TILESET_NONE;
                    cell->m_objectIndex = -1;
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_TOWN):
                    RandomizeTown(x, y, 0);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_CASTLE):
                    RandomizeTown(x, y, 1);
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER):
                    minValue = 80;
                    maxValue = 2000;
                    goto randomMonster;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_WEAK):
                    minValue = 0;
                    maxValue = 400;
                    goto randomMonster;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_MEDIUM):
                    minValue = 400;
                    maxValue = 1000;
                    goto randomMonster;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_STRONG):
                    minValue = 1000;
                    maxValue = 2500;
                    goto randomMonster;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG):
                    minValue = 2500;
                    maxValue = 100000;
                    goto randomMonster;
                randomMonster:
                    if (cell->m_objectTileset == TILESET_MONS32
                        && cell->m_objectIndex >= RANDOM_MONSTER_SPRITE_FIRST
                        && cell->m_objectIndex <= RANDOM_MONSTER_SPRITE_LAST) {
                        randomObjectType = MapTriggerCode(
                            cell->m_objectIndex + RANDOM_MONSTER_SPRITE_TO_TRIGGER
                        );
                        switch (randomObjectType) {
                            case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_WEAK):
                                minValue = 0;
                                maxValue = 400;
                                goto monsterBoundsReady;
                            case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_MEDIUM):
                                minValue = 400;
                                maxValue = 1000;
                                goto monsterBoundsReady;
                            case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_STRONG):
                                minValue = 1000;
                                maxValue = 2500;
                                goto monsterBoundsReady;
                            case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG):
                                minValue = 2500;
                                maxValue = 100000;
                                goto monsterBoundsReady;
                        }
                    }
                monsterBoundsReady:
                    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER);
                    cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                    while (gMonsterDatabase[cell->m_objectIndex].randomValue <= minValue
                           || gMonsterDatabase[cell->m_objectIndex].randomValue >= maxValue)
                        cell->m_objectIndex = static_cast<u8>(Random(0, 65));
                    break;
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_RESOURCE):
                    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_RESOURCE);
                    randomType = Random(0, 6);
                    ConvertObject(
                        x - 1,
                        y,
                        x - 1,
                        y,
                        TILESET_OBJNRSRC,
                        16,
                        16,
                        TILESET_OBJNRSRC,
                        randomType * 2,
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
                        randomType * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    switch (randomType) {
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
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ARTIFACT):
                    artifactId = GetRandomArtifactId(ARTIFACT_LEVEL_RANDOM, false);
                    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_ARTIFACT);
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
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_TREASURE_ARTIFACT):
                    artifactId = GetRandomArtifactId(ARTIFACT_LEVEL_TREASURE, false);
                    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_ARTIFACT);
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
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MINOR_ARTIFACT):
                    artifactId = GetRandomArtifactId(ARTIFACT_LEVEL_MINOR, false);
                    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_ARTIFACT);
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
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MAJOR_ARTIFACT):
                    artifactId = GetRandomArtifactId(ARTIFACT_LEVEL_MAJOR, false);
                    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_ARTIFACT);
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
                case MAP_ACTION_TRIGGER(MAP_OBJECT_EYE_OF_MAGI):
                    RandomizeMine(x, y);
                    break;
            }
        }
    }
}
#if H2_RETAIL_COMPILER
#undef mineIndex
#undef randomObjectType
#undef randomType
#endif

#if H2_RETAIL_COMPILER
#define visibilityRange vis
#endif
VA(0x0045b736, 0x20e)
void game::SetVisibility(i32 x, i32 y, i32 player, i32 radius) {
    i32 i;
    i32 cutoff;
    i32 j;
    u8 mask = static_cast<u8>(1 << player);
    i32 visibilityRange;
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
                visibilityRange = radius - abs(y - j) + radius - abs(x - i);
                if (visibilityRange >= cutoff && i >= 0 && j >= 0 && i < MAP_WIDTH
                    && j < MAP_HEIGHT) {
                    MAP_EXTRA_AT_WFIRST(i, j) |= mask;
                }
            }
        }
    }
}
#if H2_RETAIL_COMPILER
#undef visibilityRange
#endif

VA(0x0045b944, 0xc8)
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
}

VA(0x0045ba0c, 0xc8)
void game::GiveArmy(
    armyGroup* group,
    H2_ENUM_PARAM(CreatureType, i32) type,
    i32 count,
    i32 slot
) {
    i32 H2_UNUSED(swap);
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

#if H2_RETAIL_COMPILER
#define experience exp
#define heroPointer h
#endif
VA(0x0045bad4, 0x7e)
i32 game::ExperienceValueOfStack(armyGroup* group, hero* heroPointer) {
    i32 experience = 0;
    i32 i;
    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        if (group->m_quantities[i] > 0) {
            experience += group->m_quantities[i]
                * gMonsterDatabase[IDX(group->m_creatureTypes[i])].hitPoints;
        }
    }
    if (heroPointer != NULL)
        experience += EXPERIENCE_HERO_PRESENCE_BONUS;
    return experience;
}
#if H2_RETAIL_COMPILER
#undef experience
#undef heroPointer
#endif

#if H2_RETAIL_COMPILER
#define heroPointer h
#endif
VA(0x0045bb52, 0x124)
i32 game::GetLuck(hero* heroPointer, class army*, town* castle) {
    i32 luck;
    if (heroPointer == NULL)
        return NEUTRAL;
    luck = NEUTRAL;
    if (heroPointer->HasArtifact(ARTIFACT_RABBIT_FOOT))
        luck++;
    if (heroPointer->HasArtifact(ARTIFACT_GOLDEN_HORSESHOE))
        luck++;
    if (heroPointer->HasArtifact(ARTIFACT_GAMBLERS_COIN))
        luck++;
    if (heroPointer->HasArtifact(ARTIFACT_FOUR_LEAF_CLOVER))
        luck++;
    if (heroPointer->HasArtifact(ARTIFACT_MASTHEAD) && heroPointer->IsEmbarked()) {
        luck++;
    }
    luck += heroPointer->m_luck;
    luck += IDX(heroPointer->m_secondarySkills[IDX(HERO_SKILL_LUCK)]);
    if (castle != NULL && castle->m_type == FACTION_SORCERESS
        && HAS(castle->m_buildings, IDX(TOWN_BUILDING_RAINBOW))) {
        luck += RAINBOW_BONUS;
    }
    if (luck < MINIMUM)
        luck = MINIMUM;
    if (luck > MAXIMUM)
        luck = MAXIMUM;
    if (heroPointer->HasArtifact(ARTIFACT_BATTLE_GARB))
        luck = MAXIMUM;
    return luck;
}
#if H2_RETAIL_COMPILER
#undef heroPointer
#endif

#if H2_RETAIL_COMPILER
#define monsterX col2
#define monsterY row13
#endif
VA(0x0045bc76, 0xdf)
void game::SetupAdjacentMons(void) {
    i32 monsterX;
    i32 monsterY;
    u8 mask = IDX(MAP_EXTRA_ADJACENT_CLEAR_MASK);
    i32 x;
    i32 y;

    for (x = 0; x < MAP_WIDTH; x++) {
        for (y = 0; y < MAP_HEIGHT; y++) {
            if (gpAdvManager->FindAdjacentMonster(x, y, &monsterX, &monsterY, -1, -1))
                MAP_EXTRA_AT(x, y) |= IDX(MAP_EXTRA_ADJACENT_MONSTER);
            else
                MAP_EXTRA_AT(x, y) &= IDX(mask);
        }
    }
}
#if H2_RETAIL_COMPILER
#undef monsterX
#undef monsterY
#endif

VA(0x0045bd55, 0x54)
void game::CancelComputerScreen(void) {
    TurnOffAIMusic();
    bShowIt = true;
    i32 i;
    for (i = COMPUTER_SCREEN_WIDGET_FIRST; i <= COMPUTER_SCREEN_WIDGET_LAST; i++) {
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_CLEAR_FLAGS,
            i,
            IDX(WIDGET_FLAG_UPDATE | WIDGET_FLAG_DIMMED)
        );
    }
}

VA(0x0045bda9, 0xe1)
void game::ShowComputerScreen(void) {
    if (gConfig.blackoutComputer) {
        b32 saved = gbThisNetHumanPlayer[giCurPlayer];
        gbThisNetHumanPlayer[giCurPlayer] = true;
        i32 i;
        for (i = COMPUTER_SCREEN_WIDGET_FIRST; i <= COMPUTER_SCREEN_WIDGET_LAST; i++)
            gpWindowManager->BroadcastMessage(
                MESSAGE_WIDGET,
                WIDGET_COMMAND_SET_FLAGS,
                i,
                IDX(WIDGET_FLAG_UPDATE | WIDGET_FLAG_DIMMED)
            );
        gbAllBlack = true;
        gpAdvManager->CompleteDraw(1);
        gpAdvManager->UpdateHeroLocators(1, 1);
        gpAdvManager->UpdateTownLocators(1, 1);
        gpAdvManager->UpdBottomView(true, true, true);
        gpAdvManager->UpdateScreen(0, 1);
        gbAllBlack = false;
        gbThisNetHumanPlayer[giCurPlayer] = saved;
    }
    ShowHeroesLogo();
}

VA(0x0045be8a, 0x95)
void game::ShowHeroesLogo(void) {
    if (gpAdvManager->m_openState == 0) {
        gpAdvManager->m_openState = true;
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

VA(0x0045bf1f, 0x11f)
void game::WaitForPlayer(char* text, i32 player) {
    if (gbBlackoutPlayer && giNumHumanPlayers > 1 && !gbRemoteOn) {
        gpMouseManager->SetPointer(0);
        gbAllBlack = true;
        giBottomViewOverrideEndTime = KBTickCount() + WAIT_BOTTOM_VIEW_TIMEOUT;
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

#if H2_RETAIL_COMPILER
#define column col
#endif
VA(0x0045c03e, 0xda)
i32 game::HasLateOverlay(i32 column, i32 row) {
    mapCell* cell = WORLDMAP->GetCell(column, row);
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
#if H2_RETAIL_COMPILER
#undef column
#endif

#if H2_RETAIL_COMPILER
#define column col
#endif
VA(0x0045c118, 0xe9)
void game::ConvertFlagToLateOverlay(i32 column, i32 row) {
    mapCell* cell = WORLDMAP->GetCell(column, row);
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
#if H2_RETAIL_COMPILER
#undef column
#endif

#if H2_RETAIL_COMPILER
#define column col
#endif
VA(0x0045c201, 0xf7)
i32 game::HasObjectTilesetIndex(
    i32 column,
    i32 row,
    H2_ENUM_PARAM(TilesetId, i32) tileset,
    i32 index
) {
    mapCell* cell = WORLDMAP->GetCell(column, row);
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
#if H2_RETAIL_COMPILER
#undef column
#endif

#if H2_RETAIL_COMPILER
#define column col
#endif
VA(0x0045c2f8, 0xdc)
void game::ConvertAllToLateOverlay(i32 column, i32 row) {
    mapCell* cell = WORLDMAP->GetCell(column, row);
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
#if H2_RETAIL_COMPILER
#undef column
#endif

#if H2_RETAIL_COMPILER
#define cell cell10
#define mapX col6
#define mapY row16
#endif
VA(0x0045c3d4, 0x24f)
void game::ProcessMapExtra(void) {
    i32 H2_UNUSED(unused);
    i32 H2_UNUSED(cost);
    mapCell* cell;
    i32 townId;
    i32 mapY;
    i32 mapX;

    for (mapY = 0; mapY < MAP_HEIGHT; mapY++) {
        for (mapX = 0; mapX < MAP_WIDTH; mapX++) {
            cell = WORLDMAP->GetCell(mapX, mapY);
            switch (cell->m_triggerType) {
                case MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE):
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_TOWN):
                case MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_CASTLE):
                    townId = GetTownId(mapX, mapY);
                    m_castleRecs[townId].m_extraIndex = cell->m_objectMetadata;
                    cell->m_objectMetadata = townId;
                    break;
            }
        }
    }

    for (mapY = 0; mapY < MAP_HEIGHT; mapY++) {
        for (mapX = 0; mapX < MAP_WIDTH; mapX++) {
            cell = WORLDMAP->GetCell(mapX, mapY);
            if (cell->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_MINE)) && mapY > 0
                && HasLateOverlay(mapX, mapY - 1)) {
                ConvertFlagToLateOverlay(mapX, mapY);
            }
            if (cell->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_ALCHEMIST_LAB))) {
                if (mapY > 0)
                    ConvertFlagToLateOverlay(mapX, mapY - 1);
                if (mapY > 1)
                    ConvertFlagToLateOverlay(mapX, mapY - ALCHEMIST_LATE_OVERLAY_OFFSET);
            }
        }
    }

    for (mapY = 0; mapY < MAP_HEIGHT; mapY++) {
        for (mapX = 0; mapX < MAP_WIDTH; mapX++) {
            if (HasLateOverlay(mapX, mapY))
                ConvertAllToLateOverlay(mapX, mapY);
        }
    }
}
#if H2_RETAIL_COMPILER
#undef cell
#undef mapX
#undef mapY
#endif

#if H2_RETAIL_COMPILER
#define attempts attempts17
#define castle castle8
#define combatSpells combatSpells27
#define defaultDwellingRoll defaultDwellingRoll15
#define dwellingCount dwellingCount1
#define extra extra0
#define extraIndex extraIndex27
#define owner owner12
#define roll roll8
#define slot slot12
#define spell spell1
#define spellIndex spellIndex3
#define spellsPerLevel spellsPerLevel12
#define townIndex townIndex1
#define usedSpells usedSpells0
#endif
VA(0x0045c623, 0x871)
void game::SetupTowns(void) {
    u32 extraIndex;
    town* castle;
    i32 townIndex;
    i32 slot;
    i32 H2_UNUSED(unused);
    mapTownExtra* extra;
    i32 attempts;
    i8 usedSpells[IDX(SPELL_COUNT)];
    i32 owner;
    i32 combatSpells;
    i32 roll;
    SpellType spell;
    i32 spellLevel;
    i32 spellIndex;
    i32 spellsPerLevel[TOWN_MAGE_GUILD_LEVEL_COUNT];
    char defaultDwellingRoll[DEFAULT_DWELLING_ROLL_CAPACITY];
    i32 dwellingCount;
    i32 spellValue;

    for (townIndex = 0; townIndex < GAME_TOWN_COUNT; townIndex++) {
        if (!m_castleRecs[townIndex].m_onMap)
            continue;
        castle = GetTown(townIndex);

        extraIndex = castle->m_extraIndex;
        extra = reinterpret_cast<mapTownExtra*>(ppMapExtra[extraIndex]);
        if (extra->color == -1)
            owner = -1;
        else
            owner = gcColorToPlayerPos[extra->color];
        ClaimTown(townIndex, owner, 0);
        castle->m_originalOwner = static_cast<i8>(owner);

        if (extra->hasCustomArmy) {
            for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; slot++) {
                castle->m_army.m_troopCounts[slot] = extra->troopCounts[slot];
                if (static_cast<i16>(castle->m_army.m_troopCounts[slot]) > 0)
                    castle->m_army.m_troopTypes[slot] = extra->troopTypes[slot];
                else
                    castle->m_army.m_troopTypes[slot] = CREATURE_NONE;
            }
        } else {
            for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; slot++) {
                castle->m_army.m_troopCounts[slot] = 0;
                castle->m_army.m_troopTypes[slot] = CREATURE_NONE;
            }
            GiveTroopsToNeutralTown(townIndex);
            GiveTroopsToNeutralTown(townIndex);
            GiveTroopsToNeutralTown(townIndex);
            GiveTroopsToNeutralTown(townIndex);
        }

        if (extra->hasCustomBuildings) {
            castle->m_buildings =
                HAS(castle->m_buildings, (IDX(TOWN_BUILDING_CASTLE) | IDX(TOWN_BUILDING_TENT)))
                | (extra->buildings & gTownEligibleBuildMask[IDX(castle->m_type)]);
            castle->m_buildState = extra->mageGuildLevel;
        } else {
            defaultDwellingRoll[0] = 1;
            defaultDwellingRoll[1] = 1;
            defaultDwellingRoll[2] = 1;
            defaultDwellingRoll[3] = 2;
            defaultDwellingRoll[4] = 1;
            defaultDwellingRoll[5] = 1;
            defaultDwellingRoll[6] = 1;
            defaultDwellingRoll[7] = 2;
            defaultDwellingRoll[8] = 1;
            defaultDwellingRoll[9] = 2;
            dwellingCount =
                defaultDwellingRoll[Random(0, 99) / DEFAULT_DWELLING_ROLL_BUCKET_COUNT];
            castle->m_buildings |= IDX(TOWN_BUILDING_DWELLING_1);
            if (!gbHumanPlayer[IDX(castle->m_owner)] && dwellingCount == 1 && Random(1, 10) < 4)
                dwellingCount++;
            if (--dwellingCount != 0) {
                castle->m_buildings |= IDX(TOWN_BUILDING_DWELLING_2);
                dwellingCount--;
            }
            castle->m_buildState = 0;
        }

        for (slot = TOWN_UPGRADE_BUILDING_FIRST; slot <= TOWN_UPGRADE_BUILDING_LAST;
             slot++) {
            if (HAS(castle->m_buildings, (1 << slot))) {
                if (slot == TOWN_UPGRADE_BUILDING_LAST)
                    castle->m_buildings &=
                        ~(IDX(TOWN_BUILDING_DWELLING_6)
                          | IDX(TOWN_BUILDING_UPGRADED_DWELLING_6));
                else
                    castle->m_buildings &=
                        -1 - (1 << (slot - TOWN_UPGRADE_TO_DWELLING_OFFSET));
            }
        }
        for (slot = TOWN_DWELLING_BUILDING_FIRST;
             slot <= TOWN_DWELLING_BUILDING_LAST;
             slot++) {
            if (HAS(castle->m_buildings, (1 << slot))) {
                castle->m_garrison[slot - TOWN_DWELLING_BUILDING_FIRST] =
                    gMonsterDatabase[IDX(
                        gDwellingType[IDX(castle->m_type)]
                                      [slot - TOWN_DWELLING_BUILDING_FIRST]
                    )]
                        .growth;
            }
        }
        if (HAS(castle->m_buildings, IDX(TOWN_BUILDING_MAGE_GUILD))) {
            for (slot = 1; slot <= castle->m_buildState; slot++) {
                castle->m_spellCounts[slot] = gSpellLimits[slot - 1];
                if (castle->m_type == FACTION_WIZARD
                    && HAS(castle->m_buildings, BIT(BUILDING_SLOT_SPECIAL)))
                    castle->m_spellCounts[slot]++;
            }
        }
        if (extra->hasShrine)
            castle->m_buildings |= IDX(TOWN_BUILDING_CAPTAIN_QUARTERS);
        castle->m_mayNotUpgradeToCastle = extra->unknown28;
        strcpy(castle->m_name, extra->name);

        memset(usedSpells, 0, IDX(SPELL_COUNT));
        for (spellLevel = 0; spellLevel < TOWN_MAGE_GUILD_LEVEL_COUNT; spellLevel++) {
            spellsPerLevel[spellLevel] = 0;
            for (spellIndex = 0; spellIndex < TOWN_MAGE_GUILD_SPELLS_PER_LEVEL; spellIndex++)
                castle->m_spellSlots[spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellIndex] =
                    SPELL_NONE;
        }

        if (castle->m_type == FACTION_NECROMANCER && castle->m_owner != -1
            && !gbHumanPlayer[IDX(castle->m_owner)]) {
            if (Random(0, 100) < 50)
                spellIndex = IDX(SPELL_DEATH_RIPPLE);
            else
                spellIndex = IDX(SPELL_DEATH_WAVE);
            spellLevel = IDX(gsSpellInfo[spellIndex].level) - 1;
            castle->m_spells[spellLevel][spellsPerLevel[spellLevel]] = SpellType(spellIndex);
            spellsPerLevel[spellLevel]++;
        }

        roll = Random(0, 100);
        if (roll < 25)
            spellIndex = IDX(SPELL_DISPEL);
        else if (roll < 50)
            spellIndex = IDX(SPELL_MASS_DISPEL);
        else if (roll < 75)
            spellIndex = IDX(SPELL_ANTI_MAGIC);
        else
            spellIndex = IDX(SPELL_CURE);
        spellLevel = IDX(gsSpellInfo[spellIndex].level) - 1;
        castle->m_spells[spellLevel][spellsPerLevel[spellLevel]] = SpellType(spellIndex);
        spellsPerLevel[spellLevel]++;

        roll = Random(0, 100);
        if (roll < 20)
            spellIndex = IDX(SPELL_MAGIC_ARROW);
        else if (roll < 40)
            spellIndex = IDX(SPELL_LIGHTNING_BOLT);
        else if (roll < 60)
            spellIndex = IDX(SPELL_FIREBALL);
        else if (roll < 80)
            spellIndex = IDX(SPELL_COLD_RAY);
        else
            spellIndex = IDX(SPELL_COLD_RING);
        spellLevel = IDX(gsSpellInfo[spellIndex].level) - 1;
        castle->m_spells[spellLevel][spellsPerLevel[spellLevel]] = SpellType(spellIndex);
        spellsPerLevel[spellLevel]++;

        for (spellLevel = 0; spellLevel < TOWN_MAGE_GUILD_LEVEL_COUNT; spellLevel++) {
            combatSpells = 0;
            for (spellIndex = 0; spellIndex < TOWN_MAGE_GUILD_SPELLS_PER_LEVEL; spellIndex++) {
                if (castle->m_spellSlots[spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellIndex]
                    != SPELL_NONE) {
                    usedSpells[IDX(
                        castle->m_spellSlots
                            [spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellIndex]
                    )] = 1;
                    continue;
                }
                {
                    attempts = 0;
                    do {
                        spell =
                            SpellType(Random(IDX(SPELL_FIREBALL), IDX(SPELL_SET_WATER_GUARDIAN)));
                        while (IDX(gsSpellInfo[IDX(spell)].level) - 1 != spellLevel)
                            spell = SpellType(
                                Random(IDX(SPELL_FIREBALL), IDX(SPELL_SET_WATER_GUARDIAN))
                            );
                        if (castle->m_owner != -1 && !gbHumanPlayer[IDX(castle->m_owner)])
                            spellValue =
                                gsSpellInfo[IDX(spell)].aiValue
                                    * (HAS(
                                           gsSpellInfo[IDX(spell)].attributes,
                                           SPELL_INFO_ATTRIBUTE_POWER
                                       )
                                           ? 4
                                           : 1)
                                + 50;
                        else
                            spellValue = 1500;
                        if (spell == SPELL_DIMENSION_DOOR)
                            spellValue = 1500;
                    } while (
                        (combatSpells == 1
                         && HAS(gsSpellInfo[IDX(spell)].attributes, SPELL_INFO_ATTRIBUTE_ADVENTURE))
                        || Random(0, 10)
                               > gsSpellInfo[IDX(spell)].raceChance[IDX(castle->m_type)]
                        || attempts++ > 500 || usedSpells[IDX(spell)]
                        || Random(1, 1500) > spellValue
                    );
                    if (HAS(gsSpellInfo[IDX(spell)].attributes, SPELL_INFO_ATTRIBUTE_ADVENTURE))
                        combatSpells++;
                    castle
                        ->m_spellSlots[spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellIndex] =
                        spell;
                    usedSpells[IDX(spell)] = 1;
                }
            }
        }
        H2_FREE(ppMapExtra[extraIndex]);
        ppMapExtra[extraIndex] = NULL;
    }
}
#if H2_RETAIL_COMPILER
#undef attempts
#undef castle
#undef combatSpells
#undef defaultDwellingRoll
#undef dwellingCount
#undef extra
#undef extraIndex
#undef owner
#undef roll
#undef slot
#undef spell
#undef spellIndex
#undef spellsPerLevel
#undef townIndex
#undef usedSpells
#endif

#if H2_RETAIL_COMPILER
#define armySlot armySlot26
#define cell cell9
#define extra extra9
#define extraIndex extraIndex1
#define heroFaction heroClass7
#define heroOwner owner1
#define isJail isJail4
#define mapHero mapHero14
#define mapX mapX36
#define mapY mapY14
#define occupiedTown occupiedTown7
#define pass pass27
#define recordPosition recordPosition14
#define townCell townCell4
#define usedHeroes usedHeroes11
#endif
VA(0x0045ce94, 0x726)
void game::ProcessOnMapHeroes(void) {
    i32 pass;
    b8 isJail;
    mapHeroExtra* extra;
    u32 extraIndex;
    i8 usedHeroes[GAME_HERO_COUNT];
    hero* mapHero;
    i32 mapX;
    i32 recordPosition;
    i32 mapY;
    mapCell* cell;
    i32 armySlot;
    mapCell* townCell;
    i32 heroId;
    i32 heroOwner;
    FactionType heroFaction;
    town* occupiedTown;
    i32 townId;

    memset(usedHeroes, 0, GAME_HERO_COUNT);
    for (pass = 0; pass < MAP_HERO_PROCESS_PASS_COUNT; pass++) {
        for (mapY = 0; mapY < MAP_HEIGHT; mapY++) {
            for (mapX = 0; mapX < MAP_WIDTH; mapX++) {
                cell = WORLDMAP->GetCell(mapX, mapY);
                if ((cell->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_HERO
                    || cell->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_JAIL))) {

                    isJail =
                        (cell->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_JAIL;
                    extraIndex = cell->m_objectMetadata;
                    extra = reinterpret_cast<mapHeroExtra*>(ppMapExtra[extraIndex]);

                    if (pass == MAP_HERO_ASSIGNMENT_PASS) {
                        if (extra->hasCustomHero && extra->heroId < GAME_HERO_COUNT
                            && !usedHeroes[extra->heroId]) {
                            usedHeroes[extra->heroId] = 1;
                            extra->hasAssignedHero = true;
                        } else {
                            extra->hasAssignedHero = false;
                        }
                        if (isJail) {
                            extra->owner = -1;
                        } else {
                            extra->owner =
                                static_cast<i8>(cell->m_objectIndex / MAP_HERO_FRAME_STRIDE);
                            heroOwner = gcColorToPlayerPos[extra->owner];
                            extra->owner = static_cast<i8>(heroOwner);
                        }
                    }

                    if (pass == MAP_HERO_CLASS_PASS) {
                        if (isJail) {
                            heroFaction = extra->heroClass;
                        } else {
                            heroFaction = static_cast<FactionType>(
                                cell->m_objectIndex % MAP_HERO_FRAME_STRIDE
                            );
                            if (heroFaction == FACTION_NEUTRAL) {
                                heroFaction = m_setupPlayerRace
                                    [gcColorToSetupPos[gpGame->m_players[extra->owner].m_color]];
                            }
                        }

                        if (extra->hasAssignedHero) {
                            mapHero = GetHero(extra->heroId);
                            mapHero->m_cursorType = heroFaction;
                        } else {
                            heroId = RandomScan(
                                usedHeroes,
                                IDX(heroFaction) * MAP_HEROES_PER_FACTION,
                                MAP_HEROES_PER_FACTION,
                                MAP_HERO_CLASS_SCAN_RETRY_LIMIT,
                                0
                            );
                            if (heroId == -1) {
                                heroId = RandomScan(
                                    usedHeroes,
                                    0,
                                    GAME_HERO_COUNT,
                                    RANDOM_SCAN_RETRY_LIMIT,
                                    0
                                );
                                heroFaction =
                                    static_cast<FactionType>(heroId / MAP_HEROES_PER_FACTION);
                            }
                            usedHeroes[heroId] = 1;
                            mapHero = GetHero(heroId);
                            mapHero->m_cursorType = heroFaction;
                            if (extra->hasCustomHero && extra->heroId >= GAME_HERO_COUNT)
                                mapHero->m_portrait = extra->heroId;
                            extra->heroId = static_cast<i8>(heroId);
                        }
                    }

                    if (pass == MAP_HERO_PLACEMENT_PASS) {
                        mapHero = GetHero(extra->heroId);
                        if (!isJail && extra->hasPatrol) {
                            mapHero->m_patrolX = static_cast<i8>(mapX);
                            mapHero->m_patrolY = static_cast<i8>(mapY);
                            mapHero->m_patrolRadius = extra->patrolRadius;
                        }
                        if (extra->hasCustomArmy) {
                            for (armySlot = 0; armySlot < EVENT_RECORD_ARMY_SLOT_COUNT;
                                 armySlot++) {
                                mapHero->m_army.m_troopCounts[armySlot] =
                                    extra->troopCounts[armySlot];
                                if (static_cast<i16>(mapHero->m_army.m_troopCounts[armySlot]) > 0)
                                    mapHero->m_army.m_troopTypes[armySlot] =
                                        extra->troopTypes[armySlot];
                                else
                                    mapHero->m_army.m_troopTypes[armySlot] = CREATURE_NONE;
                            }
                        }
                        for (recordPosition = 0;
                             recordPosition < EVENT_RECORD_HERO_ARTIFACT_COUNT;
                             recordPosition++) {
                            if (extra->artifacts[recordPosition] >= 0)
                                GiveArtifact(
                                    mapHero,
                                    ArtifactType(extra->artifacts[recordPosition]),
                                    true
                                );
                        }
                        if (extra->hasCustomName)
                            strcpy(mapHero->m_name, extra->name);
                        mapHero->m_experience = 0;
                        gpAdvManager->GiveExperience(mapHero, extra->experience, 1);
                        mapHero->CheckLevel();
                        mapHero->m_x = mapX;
                        mapHero->m_y = mapY;

                        if (isJail) {
                            mapHero->m_owner = -1;
                            m_availableHeroes[extra->heroId] = HERO_AVAILABILITY_JAILED;
                        } else {
                            mapHero->m_owner = extra->owner;
                            m_availableHeroes[extra->heroId] = mapHero->m_owner;
                            m_players[IDX(mapHero->m_owner)]
                                .m_heroIds[m_players[IDX(mapHero->m_owner)].m_heroCount] =
                                mapHero->m_id;
                            m_players[IDX(mapHero->m_owner)].m_heroCount++;
                        }

                        if (!isJail && mapY > 0) {
                            townCell = WORLDMAP->GetCell(mapX, mapY - 1);
                            if (townCell->m_triggerType
                                == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))) {
                                mapHero->m_patrolY--;
                                mapHero->m_y--;
                                townId = GetTownId(mapX, mapY - 1);
                                occupiedTown = GetTown(townId);
                                occupiedTown->m_occupyingHeroId = mapHero->m_id;
                            }
                        }

                        if (isJail) {
                            cell->m_objectMetadata = extra->heroId;
                        } else {
                            cell->m_objectTileset = TILESET_NONE;
                            cell->m_objectIndex = MAPCELL_SPRITE_NONE;
                            cell->m_objectMetadata = 0;
                            cell->m_triggerType = 0;
                        }

                        if (extra->hasCustomSkills) {
                            mapHero->m_secondarySkillCount = 0;
                            for (recordPosition = 0;
                                 recordPosition < IDX(HERO_SKILL_COUNT);
                                 recordPosition++) {
                                mapHero->m_secondarySkills[recordPosition] =
                                    HERO_SKILL_LEVEL_NONE;
                                mapHero->m_secondarySkillOrder[recordPosition] = 0;
                            }
                            for (recordPosition = 0;
                                 recordPosition < EVENT_RECORD_SKILL_CAPACITY;
                                 recordPosition++) {
                                if (extra->skillTypes[recordPosition] != -1) {
                                    mapHero->GiveSS(
                                        static_cast<HeroSecondarySkill>(
                                            extra->skillTypes[recordPosition]
                                        ),
                                        static_cast<HeroSkillLevel>(
                                            extra->skillLevels[recordPosition]
                                        )
                                    );
                                }
                            }
                        }
                        if (!isJail) {
                            SetVisibility(
                                mapHero->m_x,
                                mapHero->m_y,
                                mapHero->m_owner,
                                giVisRange[IDX(mapHero->m_secondarySkills
                                                   [MAP_HERO_SCOUTING_SKILL_INDEX])]
                            );
                        }
                        H2_FREE(ppMapExtra[extraIndex]);
                        ppMapExtra[extraIndex] = NULL;
                    }
                }
            }
        }
    }
}
#if H2_RETAIL_COMPILER
#undef armySlot
#undef cell
#undef extra
#undef extraIndex
#undef heroFaction
#undef heroOwner
#undef isJail
#undef mapHero
#undef mapX
#undef mapY
#undef occupiedTown
#undef pass
#undef recordPosition
#undef townCell
#undef usedHeroes
#endif

#if H2_RETAIL_COMPILER
#define mapX c
#define mapY y8
#define totalHeroes all
#endif
VA(0x0045d5ba, 0x4c9)
void game::CheckHeroConsistency(void) {
    i32 totalHeroes = 0;
    i32 mapY;
    i32 mapX;
    hero* boardHro;
    i32 slot;
    i32 player;
    mapCell* cell;
    town* townOccupied;
    i32 H2_UNUSED(sane);

    for (player = 0; player < m_playerCount; player++) {
        if (m_playerDead[player] != 0)
            continue;
        totalHeroes += m_players[player].m_heroCount;
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
                    || (totalHeroes < HERO_CONSISTENCY_POOL_THRESHOLD
                        && m_availableHeroes[m_players[player].m_availableHeroIds[slot]] == -1)) {
                    m_players[player].m_availableHeroIds[slot] =
                        static_cast<i8>(GetNewHeroId(player, FACTION_ANY, 0));
                    m_availableHeroes[m_players[player].m_availableHeroIds[slot]] =
                        WEEKLY_AVAILABLE_HERO;
                }
            }
        }
    }

    for (mapX = 0; mapX < MAP_WIDTH; mapX++) {
        for (mapY = 0; mapY < MAP_HEIGHT; mapY++) {
            cell = gpAdvManager->GetCell(mapX, mapY);
            if (cell->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_MERMAID))) {
                if (cell->m_objectMetadata >= 0 && cell->m_objectMetadata < GAME_HERO_COUNT) {
                    boardHro = GetHero(cell->m_objectMetadata);
                    if (boardHro->m_x != mapX || boardHro->m_y != mapY) {
                        cell->m_triggerType = 0;
                        cell->m_objectMetadata = 0;
                    }
                    if (boardHro->m_owner < 0 || boardHro->m_owner >= GAME_PLAYER_COUNT) {
                        if (boardHro->m_locationType
                            == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))) {
                            townOccupied = gpGame->GetTown(boardHro->m_occupiedTown);
                            townOccupied->m_occupyingHeroId = -1;
                        }
                        if (boardHro->m_x == mapX && boardHro->m_y == mapY) {
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
#if H2_RETAIL_COMPILER
#undef mapX
#undef mapY
#undef totalHeroes
#endif

#if H2_RETAIL_COMPILER
#define acknowledged acknowledged7
#define batch batch9
#define done done9
#define file file7
#define fileCrc fileCrc18
#define fileData fileData5
#define fileSize fileSize15
#define filename filename18
#define header header2
#define oldTrack oldTrack6
#define packet packet0
#define packetCount packetCount4
#define packetsInBatch packetsInBatch7
#define reply reply36
#define result result9
#define samplesReady samplesReady3
#define success success7
#define transmitCrc transmitCrc9
#define unusedValue1 unused1d0
#define unusedValue2 unused208
#define unusedValue3 unused8
#define unusedValue4 unused9
#define unusedValue5 unused21c
#endif
VA(0x0045da83, 0x7b3)
i32 game::TransmitSaveGame(i32 remotePlayer, i32 player, i32 useCurrentSave) {
    char filename[TRANSMIT_FILENAME_CAPACITY];
    u32 transmitCrc;
    i32 packetsInBatch;
    i32 H2_UNUSED(unusedValue1);
    u32 fileCrc;
    i32 oldTrack;
    i32 batchCount;
    i32 chunkSize;
    i32 result;
    i32 packetCount;
    char* reply;
    b32 success;
    i32 fileSize;
    i32 packet;
    u8* transmitData;
    i32 file;
    i32 H2_UNUSED(unusedValue2);
    b32 done;
    i32 H2_UNUSED(unusedValue5);
    bool samplesReady;
    u8* fileData;
    char* acknowledged;
    i32* header;
    i32 H2_UNUSED(unusedValue3);
    i32 batch;
    i32 H2_UNUSED(unusedValue4);

    gpAdvManager->TrimLoopingSounds(REMOTE_LOOPING_SOUND_COUNT);
    header = NULL;
    reply = NULL;
    transmitData = NULL;
    fileData = NULL;
    success = false;
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
    LogInt(const_cast<char*>("PostDiffFileSize"), fileSize);

    header = static_cast<i32*>(H2_ALLOC(REMOTE_HEADER_CAPACITY));
    if (gbUseRegularCompression)
        transmitData = static_cast<u8*>(H2_ALLOC(fileSize + REMOTE_BUFFER_EXTRA));
    fileData = static_cast<u8*>(H2_ALLOC(fileSize + REMOTE_BUFFER_EXTRA));

    file = open(filename, _O_BINARY);
    if (file == -1)
        FileError(filename);
    if (file == -1) {
        goto transmitCleanup;
    }
    {
        read(file, fileData, fileSize);
        close(file);
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
        LogInt(const_cast<char*>("Send"), fileSize, transmitCrc);

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

            done = false;
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
                            0
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
                done = true;
                for (packet = batch * REMOTE_PACKET_BATCH_SIZE;
                     packet < batch * REMOTE_PACKET_BATCH_SIZE + packetsInBatch;
                     packet++) {
                    if (!acknowledged[packet])
                        done = false;
                }
            }
        }
        result = TransmitRemoteData(NULL, remotePlayer, 0, REMOTE_SAVE_FINISH_COMMAND, 1);
        if (!result)
            ShutDown(NULL);
        success = true;
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
        gpAdvManager->UpdBottomView(true, true, true);
    }
    if (oldTrack != -1) {
        samplesReady = gSoundBackendsReady;
        gSoundBackendsReady = 1;
        gpSoundManager->SwitchAmbientMusic(oldTrack);
        gSoundBackendsReady = samplesReady;
    }
    return success;
}
#if H2_RETAIL_COMPILER
#undef acknowledged
#undef batch
#undef done
#undef file
#undef fileCrc
#undef fileData
#undef fileSize
#undef filename
#undef header
#undef oldTrack
#undef packet
#undef packetCount
#undef packetsInBatch
#undef reply
#undef result
#undef samplesReady
#undef success
#undef transmitCrc
#undef unusedValue1
#undef unusedValue2
#undef unusedValue3
#undef unusedValue4
#undef unusedValue5
#endif

#if H2_RETAIL_COMPILER
#define ackBuffer ackBuffer6
#define computedCrc computedCrc2
#define file file6
#define filename filename18
#define finished finished4
#define incomingData incomingData7
#define index index27
#define lastPacketTime lastPacketTime9
#define oldTrack oldTrack6
#define packet packet15
#define packetStart packetStart0
#define result result9
#define samplesReady samplesReady0
#define success success7
#define unusedValue unused2080
#endif
VA(0x0045e236, 0x6c8)
i32 game::ReceiveSaveGame(
    i32 dataSize,
    i32 expectedCrc,
    i32 expectedTransmitCrc,
    i32 remotePlayer
) {
    char filename[RECEIVE_FILENAME_CAPACITY];
    i32 receivedCrc;
    b32 finished;
    i32 oldTrack;
    i32 result;
    char* received;
    RemoteMessage* packet;
    i32 computedCrc;
    b32 success;
    i32 index;
    u8* incomingData;
    i32 file;
    i32 packetStart;
    u8* ackBuffer;
    u8* decodedData;
    bool samplesReady;
    i32 H2_UNUSED(unusedValue);
    i32l lastPacketTime;

    LogInt(const_cast<char*>("FW1"), remotePlayer);
    LogStr(const_cast<char*>("RSG1"));
    AiPrint(const_cast<char*>("Receive Start - Getting Data"));
    gpAdvManager->TrimLoopingSounds(REMOTE_LOOPING_SOUND_COUNT);

    ackBuffer = NULL;
    incomingData = NULL;
    decodedData = NULL;
    packet = NULL;
    file = 0;
    finished = false;
    unusedValue = 0;
    received = NULL;
    success = false;
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
    result = TransmitRemoteData(NULL, remotePlayer, 0, REMOTE_SAVE_INIT_RESPONSE, 1);
    LogStr(const_cast<char*>("End Transmit Init Confirm"));
    if (!result)
        ShutDown(NULL);

    received = static_cast<char*>(H2_ALLOC(REMOTE_PACKET_TRACKING_CAPACITY));
    memset(received, 0, REMOTE_PACKET_TRACKING_CAPACITY);
    if (gbUseRegularCompression)
        decodedData = static_cast<u8*>(H2_ALLOC(REMOTE_DECODE_BUFFER_SIZE));
    ackBuffer = static_cast<u8*>(H2_ALLOC(REMOTE_HEADER_CAPACITY));
    incomingData = static_cast<u8*>(H2_ALLOC(dataSize + REMOTE_BUFFER_EXTRA));

    lastPacketTime = KBTickCount();
    LogInt(const_cast<char*>("FW2"), remotePlayer);
    while (!finished) {
        PollSound();
        CheckDoMain(0, 1);
        if (lastPacketTime + REMOTE_RECEIVE_TIMEOUT < KBTickCount()) {
            NormalDialog(
                const_cast<char*>(localization::Tr("network.receive.retry")),
                REMOTE_RECEIVE_DIALOG_BUTTONS
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE)
                lastPacketTime = KBTickCount();
            else
                ShutDown(NULL);
        }

        packet = reinterpret_cast<RemoteMessage*>(GetRemoteData(1));
        if (packet
            && (packet->type == REMOTE_MESSAGE_RELIABLE
                || packet->type == REMOTE_MESSAGE_UNRELIABLE)) {
            lastPacketTime = KBTickCount();
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
                    LogInt(const_cast<char*>("FW3"), remotePlayer);
                    result = TransmitRemoteData(
                        reinterpret_cast<char*>(ackBuffer),
                        remotePlayer,
                        REMOTE_PACKET_PAYLOAD_SIZE,
                        REMOTE_SAVE_ACK_RESPONSE_COMMAND,
                        1
                    );
                    if (!result)
                        ShutDown(NULL);
                    break;
                case REMOTE_SAVE_FINISH_COMMAND:
                    finished = true;
                    break;
            }
        }
    }

    AiPrint(const_cast<char*>("Receive Start - Decompressing Data"));
    receivedCrc = calc_crc_long(incomingData, dataSize);
    LogInt(const_cast<char*>("Receive"), dataSize, receivedCrc, expectedTransmitCrc);
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
    LogInt(const_cast<char*>("Receive"), dataSize, computedCrc, expectedCrc);

    sprintf(filename, "%s%s", ".\\DATA\\", gConfig.rmtRDName);
    file = open(filename, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (file == -1)
        FileError(filename);
    write(file, decodedData, dataSize);
    close(file);
    success = true;

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
        gpAdvManager->UpdBottomView(true, true, true);
    }
    if (oldTrack != -1) {
        samplesReady = gSoundBackendsReady;
        gSoundBackendsReady = 1;
        gpSoundManager->SwitchAmbientMusic(oldTrack);
        gSoundBackendsReady = samplesReady;
    }
    return success;
}
#if H2_RETAIL_COMPILER
#undef ackBuffer
#undef computedCrc
#undef file
#undef filename
#undef finished
#undef incomingData
#undef index
#undef lastPacketTime
#undef oldTrack
#undef packet
#undef packetStart
#undef result
#undef samplesReady
#undef success
#undef unusedValue
#endif

#if H2_RETAIL_COMPILER
#define lowerName lowerName19
#define musicFile musicFile18
#define musicTrack musicTrack2
#endif
VA(0x0045e8fe, 0x4fd)
void game::DoNewTurn(void) {
    char musicFile[NEW_TURN_MUSIC_FILENAME_CAPACITY];
    u8 lowerName[NEW_TURN_LOWER_NAME_CAPACITY];
    i32 musicTrack;

    CheckForTimeEvent();
    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        CheckEndGame(END_GAME_FORCE_NONE, false);
        return;
    }
    giBottomViewOverrideEndTime = KBTickCount() + NEW_TURN_BOTTOM_VIEW_DURATION;
    giBottomViewOverride = BOTTOM_VIEW_NEW_TURN;
    gpAdvManager->UpdBottomView(true, true, true);
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
            musicTrack = -1;
            if (m_week == 1) {
                musicTrack = NEW_MONTH_MUSIC_TRACK;
                strcpy(musicFile, "newmonth.82m");
                if (giMonthType == CALENDAR_PERIOD_NORMAL) {
                    sprintf(
                        gText,
                        cNewTurn[NEW_MONTH_NORMAL_TEXT],
                        gMonthNames[giMonthTypeExtra]
                    );
                } else if (giMonthType == CALENDAR_PERIOD_CREATURE) {
                    u8 lowerFirst;
                    strcpy(
                        reinterpret_cast<char*>(lowerName),
                        gArmyNamesPlural[giMonthTypeExtra]
                    );
                    if (lowerName[0] >= 'A' && lowerName[0] <= 'Z')
                        lowerFirst = lowerName[0] + ('a' - 'A');
                    else if (lowerName[0] >= CYRILLIC_CAPITAL_A
                             && lowerName[0] <= CYRILLIC_CAPITAL_YA)
                        lowerFirst = lowerName[0] + (CYRILLIC_SMALL_A - CYRILLIC_CAPITAL_A);
                    else if (lowerName[0] == CYRILLIC_CAPITAL_YO)
                        lowerFirst = CYRILLIC_SMALL_YO;
                    else
                        lowerFirst = lowerName[0];
                    lowerName[0] = lowerFirst;
                    sprintf(
                        gText,
                        cNewTurn[NEW_MONTH_CREATURE_TEXT],
                        gArmyNamesPlural[giMonthTypeExtra],
                        reinterpret_cast<char*>(lowerName)
                    );
                } else {
                    sprintf(gText, cNewTurn[NEW_MONTH_PLAGUE_TEXT]);
                }
            } else {
                musicTrack = NEW_WEEK_MUSIC_TRACK;
                strcpy(musicFile, "newweek.82m");
                if (giWeekType == CALENDAR_PERIOD_NORMAL) {
                    sprintf(gText, cNewTurn[NEW_WEEK_NORMAL_TEXT], gWeekNames[giWeekTypeExtra]);
                } else {
                    u8 lowerFirst;
                    strcpy(
                        reinterpret_cast<char*>(lowerName),
                        gArmyNamesPlural[giWeekTypeExtra]
                    );
                    if (lowerName[0] >= 'A' && lowerName[0] <= 'Z')
                        lowerFirst = lowerName[0] + ('a' - 'A');
                    else if (lowerName[0] >= CYRILLIC_CAPITAL_A
                             && lowerName[0] <= CYRILLIC_CAPITAL_YA)
                        lowerFirst = lowerName[0] + (CYRILLIC_SMALL_A - CYRILLIC_CAPITAL_A);
                    else if (lowerName[0] == CYRILLIC_CAPITAL_YO)
                        lowerFirst = CYRILLIC_SMALL_YO;
                    else
                        lowerFirst = lowerName[0];
                    lowerName[0] = lowerFirst;
                    sprintf(
                        gText,
                        cNewTurn[NEW_WEEK_CREATURE_TEXT],
                        gArmyNamesPlural[giWeekTypeExtra],
                        reinterpret_cast<char*>(lowerName)
                    );
                }
            }
            gpSoundManager->PlayAmbientMusic(musicTrack);
            gpMouseManager->SetPointer(0);
            NormalDialog(gText, 1);
            gpSoundManager->SwitchAmbientMusic(
                giTerrainToMusicTrack[IDX(gpAdvManager->m_currentTerrain)]
            );
        }
    }
    gpSoundManager->SwitchAmbientMusic(
        giTerrainToMusicTrack[IDX(gpAdvManager->m_currentTerrain)]
    );
    gpAdvManager->SetEnvironmentOrigin(
        gpAdvManager->m_mapOriginX + ENVIRONMENT_ORIGIN_TILE_OFFSET,
        gpAdvManager->m_mapOriginY + ENVIRONMENT_ORIGIN_TILE_OFFSET,
        1
    );
}
#if H2_RETAIL_COMPILER
#undef lowerName
#undef musicFile
#undef musicTrack
#endif

VA(0x0045edfb, 0x4c)
i32 game::GetBoatsBuilt(void) {
    i32 count = 0;
    i32 i;
    for (i = 0; i < GAME_BOAT_COUNT; i++) {
        if (m_boatSlots[i] != -1)
            count++;
    }
    return count;
}

#if H2_RETAIL_COMPILER
#define count num
#endif
VA(0x0045ee47, 0x7f)
i32 game::GetNumThievesGuilds(i32 color) {
    i32 count = 0;
    i32 i;
    for (i = 0; i < m_players[color].m_townCount; i++) {
        if (HAS(gpGame->m_castleRecs[m_players[color].m_townIds[i]].m_buildings,
                IDX(TOWN_BUILDING_THIEVES_GUILD)))
            count++;
    }
    return count;
}
#if H2_RETAIL_COMPILER
#undef count
#endif

VA(0x0045eec6, 0xff)
i32 game::CalcDifficultyRating(void) {
    i32 H2_UNUSED(notused);
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

VA(0x0045efc5, 0x177)
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

#if H2_RETAIL_COMPILER
#define unused p6
#endif
VA(0x0045f13c, 0xa7)
void game::RestoreCell(
    i32 x,
    i32 y,
    H2_OPEN_CODE_PARAM(MapTriggerCode, i32) objectType,
    i32 barrier,
    mapCell* passedCell,
    i32 H2_UNUSED(unused)
) {
    mapCell* cell;
    if (passedCell)
        cell = passedCell;
    else
        cell = gpAdvManager->GetCell(x, y);
    if (y > 0 && objectType == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
        && gpAdvManager->GetCell(x, y - 1)->m_triggerType != MAP_OBJECT_CASTLE) {
        cell->m_triggerType = 0;
        cell->m_objectMetadata = 0;
        return;
    }
    cell->m_triggerType = objectType;
    cell->m_objectMetadata = barrier;
}
#if H2_RETAIL_COMPILER
#undef unused
#endif

#if H2_RETAIL_COMPILER
#define height h
#define width w
#endif
VA(0x0045f1e3, 0xb9)
void game::SetMapSize(i32 width, i32 height) {
    if (MAP_HEIGHT == height && MAP_WIDTH == width && bMapInitialized)
        goto mapSized;
    {
        bMapInitialized = true;
        MAP_WIDTH = width;
        MAP_HEIGHT = height;
        gpSearchArray->Init();
    }
mapSized:
    if (mapExtra)
        H2_FREE(mapExtra);
    mapExtra = static_cast<u8*>(H2_ALLOC(MAP_HEIGHT * MAP_WIDTH));
    memset(mapExtra, 0, MAP_HEIGHT * MAP_WIDTH);
}
#if H2_RETAIL_COMPILER
#undef height
#undef width
#endif

#if H2_RETAIL_COMPILER
#define buffer buf
#define command cmd
#define length len
#define lowByte lo
#define position pos
#endif
VA(0x0045f29c, 0x107)
void WriteDiffHeaderInfo(u8 command, i32 length, u8* buffer, i32* position) {
    u8 flags = 0;
    flags |= command << COMMAND_SHIFT;
    if (length > LEN_WORD_MAX) {
        flags |= LEN_WORD_FLAG;
        flags |= (length & LEN_HIGH_MASK) >> DIFF_WORD_SHIFT;
        u16 word = static_cast<u16>(length & LEN_LOW_MASK);
        buffer[*position] = flags;
        *reinterpret_cast<u16*>(buffer + *position + 1) = word;
        *position += DIFF_WORD_HEADER_SIZE;
    } else if (length > LEN_BYTE_MAX) {
        flags |= LEN_BYTE_FLAG;
        flags |= (length & LEN_MID_MASK) >> DIFF_BYTE_SHIFT;
        u8 lowByte = length & LEN_BYTE_MASK;
        buffer[*position] = flags;
        buffer[*position + 1] = lowByte;
        *position += DIFF_BYTE_HEADER_SIZE;
    } else {
        flags |= static_cast<u8>(length);
        buffer[*position] = flags;
        (*position)++;
    }
}
#if H2_RETAIL_COMPILER
#undef buffer
#undef command
#undef length
#undef lowByte
#undef position
#endif

#if H2_RETAIL_COMPILER
#define buffer buf
#define headerByte b
#define length len
#define position pos
#endif
VA(0x0045f3a3, 0xd7)
i32 GetSkipCopyLen(u8* buffer, i32* position) {
    u8 headerByte = buffer[*position];
    i32 length;
    if (headerByte & LEN_WORD_FLAG) {
        length = headerByte & LEN_WORD_HIGH_MASK;
        length <<= DIFF_WORD_SHIFT;
        length |= *reinterpret_cast<u16*>(buffer + *position + 1);
        *position += DIFF_WORD_HEADER_SIZE;
    } else if (headerByte & LEN_BYTE_FLAG) {
        length = headerByte & LEN_SHORT_MASK;
        length <<= DIFF_BYTE_SHIFT;
        length |= buffer[*position + 1];
        *position += DIFF_BYTE_HEADER_SIZE;
    } else {
        length = headerByte & LEN_SHORT_MASK;
        (*position)++;
    }
    return length;
}
#if H2_RETAIL_COMPILER
#undef buffer
#undef headerByte
#undef length
#undef position
#endif

#if H2_RETAIL_COMPILER
#define destinationFile destFile
#define matchLength matchLen
#endif
VA(0x0045f47a, 0x553)
void CreateDiffFile(
    char* oldName,
    char* joinName,
    char* diffName,
    i32 remotePlayer,
    i32 forceWhole
) {
    i32 joinSize;
    u8* fullData;
    i32 H2_UNUSED(unusedVal);
    i32 inFd;
    i32l H2_UNUSED(timeIn);
    i32 matchLength;
    u8* prevData;
    i32 diffTotal;
    i32 readFile;
    i32 length;
    u8* diffOut;
    i32 oldSize;
    b32 fullSend;
    i32 destinationFile;
    i32 position;

    timeIn = KBTickCount();
    prevData = NULL;
    fullData = NULL;
    diffOut = NULL;
    oldSize = 0;
    joinSize = 0;
    diffTotal = 0;
    fullSend = false;

    if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
        fullSend = true;
    iLastDiffSendTo = remotePlayer;

    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    joinSize = FileSize(gText);
    fullData = static_cast<u8*>(H2_ALLOC(joinSize));
    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    readFile = open(gText, _O_BINARY);
    if (readFile == -1)
        FileError(gText);
    read(readFile, fullData, joinSize);
    close(readFile);
    LogInt(const_cast<char*>("Orig Join CRC"), calc_crc_long(fullData, joinSize), joinSize);

    if (!forceWhole) {
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        oldSize = FileSize(gText);
        prevData = static_cast<u8*>(H2_ALLOC(oldSize));
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        inFd = open(gText, _O_BINARY);
        if (inFd == -1)
            FileError(gText);
        read(inFd, prevData, oldSize);
        close(inFd);
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
        matchLength = length;
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
                matchLength = 1;
                while (position + length + matchLength < oldSize
                       && position + length + matchLength < joinSize
                       && *(prevData + position + length + matchLength)
                              == *(fullData + position + length + matchLength))
                    matchLength++;
                if (matchLength <= DIFF_MAX_SHORT_MATCH) {
                    length += matchLength;
                    matchLength = 0;
                } else {
                    if (length != 0) {
                        WriteDiffHeaderInfo(1, length, diffOut, &diffTotal);
                        memcpy(diffOut + diffTotal, fullData + position, length);
                        diffTotal += length;
                        position += length;
                        length = 0;
                    }
                    WriteDiffHeaderInfo(0, matchLength, diffOut, &diffTotal);
                    position += matchLength;
                    matchLength = 0;
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
    destinationFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (destinationFile == -1)
        FileError(gText);
    write(destinationFile, diffOut, diffTotal);
    close(destinationFile);

    sprintf(gText, "%s%s", ".\\DATA\\", oldName);
    destinationFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (destinationFile == -1)
        FileError(gText);
    write(destinationFile, fullData, joinSize);
    close(destinationFile);

    if (prevData != NULL)
        H2_FREE(prevData);
    if (fullData != NULL)
        H2_FREE(fullData);
    if (diffOut != NULL)
        H2_FREE(diffOut);
    return;
}
#if H2_RETAIL_COMPILER
#undef destinationFile
#undef matchLength
#endif

#if H2_RETAIL_COMPILER
#define oldBuffer oldBuf
#endif
VA(0x0045f9cd, 0x37e)
void CreateJoinFile(char* oldName, char* diffName, char* joinName) {
    u8* oldBuffer = NULL;
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
    diffFile = open(gText, _O_BINARY);
    if (diffFile == -1)
        FileError(gText);
    read(diffFile, diffData, diffLength);
    close(diffFile);

    outData = static_cast<u8*>(H2_ALLOC(JOIN_BUFFER_SIZE));
    if (diffData[0] == 0) {
        memcpy(outData, diffData + JOIN_HEADER_SIZE, diffLength - JOIN_HEADER_SIZE);
        outSize = diffLength - JOIN_HEADER_SIZE;
    } else {
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        oldSize = FileSize(gText);
        oldBuffer = static_cast<u8*>(H2_ALLOC(oldSize));
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        diffFile = open(gText, _O_BINARY);
        if (diffFile == -1)
            FileError(gText);
        read(diffFile, oldBuffer, oldSize);
        close(diffFile);
        memcpy(outData, oldBuffer, oldSize);

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
    joinFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (joinFile == -1)
        FileError(gText);
    write(joinFile, outData, outSize);
    close(joinFile);
    LogInt(const_cast<char*>("New Join CRC"), calc_crc_long(outData, outSize), outSize);

    sprintf(gText, "%s%s", ".\\DATA\\", oldName);
    joinFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (joinFile == -1)
        FileError(gText);
    write(joinFile, outData, outSize);
    close(joinFile);

    if (oldBuffer)
        H2_FREE(oldBuffer);
    if (diffData)
        H2_FREE(diffData);
    if (outData)
        H2_FREE(outData);
}
#if H2_RETAIL_COMPILER
#undef oldBuffer
#endif

#if H2_RETAIL_COMPILER
#define player pd
#endif
VA(0x0045fd4b, 0x46)
i32 game::HeroIDToHeroPos(playerData* player, i32 heroId) {
    i32 i;
    for (i = 0; i < player->m_heroCount; i++) {
        if (player->m_heroIds[i] == heroId)
            return i;
    }
    return -1;
}
#if H2_RETAIL_COMPILER
#undef player
#endif

#if H2_RETAIL_COMPILER
#define player pd
#endif
VA(0x0045fd91, 0x46)
i32 game::TownIDToTownPos(playerData* player, i32 townId) {
    i32 i;
    for (i = 0; i < player->m_townCount; i++) {
        if (player->m_townIds[i] == townId)
            return i;
    }
    return -1;
}
#if H2_RETAIL_COMPILER
#undef player
#endif

#if H2_RETAIL_COMPILER
#define attempts attempts8
#define event event0
#define selectionRoll selectionRoll7
#endif
VA(0x0045fdd7, 0x64d)
void game::SetupNewRumour(void) {
    i32 roll;
    i32 attempts;
    rumourEventExtra* event;
    i32 eventIndex;
    i32 selectionRoll;
    i32l categoryStats[GAME_PLAYER_COUNT];
    i32 direction;
    i8 categoryOrder[RUMOUR_CATEGORY_ORDER_CAPACITY];
    if (m_rumourEventCount != 0 && Random(0, 9) < static_cast<i32>(m_rumourEventCount)) {
        attempts = 0;
        while (attempts++ < 200) {
            if (m_rumourEventCount > 1)
                eventIndex = Random(0, m_rumourEventCount - 1);
            else
                eventIndex = 0;
            event =
                reinterpret_cast<rumourEventExtra*>(ppMapExtra[m_rumourEventIndices[eventIndex]]);
            if (strlen(event->text) > 2 && event->text[0] != '@') {
                strcpy(m_rumour, event->text);
                event->text[0] = '@';
                return;
            }
        }
    }

    if (Random(0, 100) < 30) {
        strcpy(m_rumour, cRandomTavernText[(giCurTurn / 7) % 8]);
    } else {
        roll = Random(0, 100);
        if (roll < 80 && giCurTurn > 1) {
            attempts = 0;
            while (attempts++ < 200) {
                selectionRoll = Random(
                    IDX(THIEVES_CATEGORY_OBELISKS), IDX(THIEVES_CATEGORY_INCOME)
                );
                GetCategoryStats(
                    static_cast<TownThievesGuildCategory>(selectionRoll),
                    categoryStats,
                    categoryOrder
                );
                SortStats(categoryStats, categoryOrder);
                if (categoryStats[0] != categoryStats[1]) {
                    if (selectionRoll == IDX(THIEVES_CATEGORY_OBELISKS))
                        sprintf(
                            m_rumour,
                            localization::Tr("rumor.leader.obelisks"),
                            cPlayerNames[categoryOrder[0]]
                        );
                    else if (selectionRoll == IDX(THIEVES_CATEGORY_ARTIFACTS))
                        sprintf(
                            m_rumour,
                            localization::Tr("rumor.leader.artifacts"),
                            cPlayerNames[categoryOrder[0]]
                        );
                    else if (selectionRoll == IDX(THIEVES_CATEGORY_ARMY_STRENGTH))
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
            selectionRoll = Random(0, 100);
            if (selectionRoll < 33) {
                if (!(m_ultimateArtifactX >= IDX(m_mapHeader.width) * 0.33
                      || m_ultimateArtifactX >= IDX(m_mapHeader.height) * 0.33)) {
                    direction = 7;
                } else if (!(m_ultimateArtifactX >= IDX(m_mapHeader.width) * 0.33
                             || m_ultimateArtifactX <= IDX(m_mapHeader.height)
                                    * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS)) {
                    direction = 5;
                } else if (!(m_ultimateArtifactX >= IDX(m_mapHeader.width) * 0.33)) {
                    direction = 6;
                } else if (!(m_ultimateArtifactX <= IDX(m_mapHeader.width)
                                 * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS
                             || m_ultimateArtifactX >= IDX(m_mapHeader.height) * 0.33)) {
                    direction = 1;
                } else if (!(m_ultimateArtifactX <= IDX(m_mapHeader.width)
                                 * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS
                             || m_ultimateArtifactX <= IDX(m_mapHeader.height)
                                    * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS)) {
                    direction = 3;
                } else if (!(m_ultimateArtifactX <= IDX(m_mapHeader.width)
                                 * GAME_ULTIMATE_ARTIFACT_TWO_THIRDS)) {
                    direction = 2;
                } else if (!(m_ultimateArtifactX >= IDX(m_mapHeader.height) * 0.33)) {
                    direction = 0;
                } else if (!(m_ultimateArtifactX <= IDX(m_mapHeader.height)
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
            } else if (selectionRoll < 66) {
                sprintf(
                    m_rumour,
                    localization::Tr("rumor.ultimate_artifact.terrain"),
                    cRumourTerrainDescriptions[IDX(CELL_TERRAIN(
                        gpAdvManager->GetCell(m_ultimateArtifactX, m_ultimateArtifactY)
                    ))]
                );
            } else if (m_ultimateArtifactId != ARTIFACT_NONE) {
                sprintf(
                    m_rumour,
                    localization::Tr("rumor.ultimate_artifact.identity"),
                    gArtifactNames[IDX(m_ultimateArtifactId)]
                );
            } else {
                strcpy(m_rumour, cRandomTavernText[(giCurTurn / 7) % 8]);
            }
        }
    }
}
#if H2_RETAIL_COMPILER
#undef attempts
#undef event
#undef selectionRoll
#endif

#if H2_RETAIL_COMPILER
#define event ev
#endif
VA(0x00460424, 0xae)
EventExtra* GetMapEvent(i32 x, i32 y) {
    EventExtra* event;
    i32 i;
    for (i = 0; i < gpGame->m_mapEventCount; i++) {
        event = reinterpret_cast<EventExtra*>(ppMapExtra[gpGame->m_mapEventIndices[i]]);
        if (event->x == x && event->y == y && event->active != 0
            && event->players[gpGame->m_players[static_cast<i8>(giCurPlayer)].m_color] != 0)
            return event;
    }
    return NULL;
}
#if H2_RETAIL_COMPILER
#undef event
#endif

#if H2_RETAIL_COMPILER
#define dayNumber dayNumber4
#define event event0
#define eventIndex eventIndex5
#define primaryAmount primaryAmount2
#define primaryType primaryType9
#define secondaryType secondaryType4
#endif
VA(0x004604d2, 0x318)
void game::CheckForTimeEvent(void) {
    timeEventExtra* event;
    i32 dayNumber;
    i32 eventIndex;
    i32 resourceIndex;
    i32 primaryAmount;
    i32 primaryType;
    i32 secondaryType;
    i32 secondaryAmount;
    i32 resourceAmount;

    dayNumber = GAME_DAY_NUMBER(*this);
    for (eventIndex = 0; eventIndex < m_timeEventCount; eventIndex++) {
        event = static_cast<timeEventExtra*>(ppMapExtra[m_timeEventIndices[eventIndex]]);
        if (((gbHumanPlayer[giCurPlayer] && event->appliesToHuman)
             || (!gbHumanPlayer[giCurPlayer] && event->appliesToComputer))
            && event->players[GetPlayerColor(static_cast<i8>(giCurPlayer))]
            && (event->firstDay == dayNumber
                || (event->repeatInterval != 0 && dayNumber > event->firstDay
                    && (dayNumber - event->firstDay) % event->repeatInterval == 0))) {
            primaryType = -1;
            primaryAmount = 0;
            secondaryType = -1;
            secondaryAmount = 0;
            for (resourceIndex = 0; resourceIndex < EVENT_RESOURCE_COUNT;
                 resourceIndex++) {
                resourceAmount = event->resources[resourceIndex];
                if (-resourceAmount
                    > gpGame->m_players[giCurPlayer].m_resources[resourceIndex]) {
                    resourceAmount = -gpGame->m_players[giCurPlayer].m_resources[resourceIndex];
                }
                gpGame->m_players[giCurPlayer].m_resources[resourceIndex] +=
                    event->resources[resourceIndex];
                if (gpGame->m_players[giCurPlayer].m_resources[resourceIndex] < 0)
                    gpGame->m_players[giCurPlayer].m_resources[resourceIndex] = 0;
                if (resourceAmount != 0) {
                    if (primaryType != -1) {
                        secondaryType = primaryType;
                        secondaryAmount = primaryAmount;
                    }
                    primaryType = resourceIndex;
                    primaryAmount = resourceAmount;
                }
            }
            if (primaryType >= 0 && primaryType <= EVENT_RESOURCE_COUNT - 1
                && primaryAmount < 0) {
                primaryAmount -= EVENT_RESOURCE_PENALTY;
            }
            if (secondaryType >= 0 && secondaryType <= EVENT_RESOURCE_COUNT - 1
                && secondaryAmount < 0) {
                secondaryAmount -= EVENT_RESOURCE_PENALTY;
            }
            if (gbThisNetHumanPlayer[giCurPlayer]) {
                NormalDialog(
                    event->message,
                    1,
                    -1,
                    -1,
                    primaryType,
                    primaryAmount,
                    secondaryType,
                    secondaryAmount,
                    -1,
                    0
                );
            }
        }
    }
}
#if H2_RETAIL_COMPILER
#undef dayNumber
#undef event
#undef eventIndex
#undef primaryAmount
#undef primaryType
#undef secondaryType
#endif

#if H2_RETAIL_COMPILER
#define availableSlot availableSlot13
#define candidatePlayer candidatePlayer0
#define heroIndex heroIndex5
#define heroPlayer heroPlayer26
#endif
VA(0x004607ea, 0x11e)
void CheckValidAvailableHeroes(void) {
    i32 candidatePlayer;
    i32 heroIndex;
    i32 availableSlot;
    i32 heroPlayer;

    for (heroPlayer = 0; heroPlayer < gpGame->m_playerCount; heroPlayer++) {
        for (heroIndex = 0; heroIndex < gpGame->m_players[heroPlayer].m_heroCount;
             heroIndex++) {
            for (candidatePlayer = 0; candidatePlayer < gpGame->m_playerCount;
                 candidatePlayer++) {
                for (availableSlot = 0; availableSlot < AVAILABLE_HERO_SLOTS;
                     availableSlot++) {
                    if (gpGame->m_players[candidatePlayer].m_availableHeroIds[availableSlot]
                        == gpGame->m_players[heroPlayer].m_heroIds[heroIndex]) {
                        gpGame->m_players[candidatePlayer].m_availableHeroIds[availableSlot] =
                            static_cast<i8>(gpGame->GetNewHeroId(heroPlayer, FACTION_ANY, 0));
                    }
                }
            }
        }
    }
}
#if H2_RETAIL_COMPILER
#undef availableSlot
#undef candidatePlayer
#undef heroIndex
#undef heroPlayer
#endif

#if H2_RETAIL_COMPILER
#define block blk
#define fileDescriptor fp
#define length len
#endif
VA(0x00460908, 0x92)
i32 CalcFileCRC(char* file) {
    i32l length = FileSize(file);
    char* block = static_cast<char*>(H2_ALLOC(length));
    i32 fileDescriptor = open(file, _O_BINARY);
    if (fileDescriptor == -1)
        FileError(file);
    read(fileDescriptor, block, length);
    i32 checksum = calc_crc_long(reinterpret_cast<u8*>(block), length);
    close(fileDescriptor);
    H2_FREE(block);
    return checksum;
}
#if H2_RETAIL_COMPILER
#undef block
#undef fileDescriptor
#undef length
#endif

#if H2_RETAIL_COMPILER
#define sourceCrc srcCrc
#define sourceCrcCheck srcCrcCheck
#endif
VA(0x0046099a, 0x120)
void CompressTest2(void) {
    i32l H2_UNUSED(plainSize);
    char* unpackedData;
    i32l compSize;
    i32 H2_UNUSED(sourceCrc);
    i32 dataSz;
    i32 index;
    i32 H2_UNUSED(unpackedCrc);
    char* fromData;
    char* encoded;
    i32 H2_UNUSED(sourceCrcCheck);

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
    sourceCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
    compSize = EncodeData(encoded, fromData, dataSz);
    plainSize = DecodeData(unpackedData, encoded, compSize);
    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
    sourceCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
    H2_FREE(fromData);
    H2_FREE(encoded);
    H2_FREE(unpackedData);
}
#if H2_RETAIL_COMPILER
#undef sourceCrc
#undef sourceCrcCheck
#endif

#if H2_RETAIL_COMPILER
#define sourceCrc srcCrc
#define sourceCrcCheck srcCrcCheck
#endif
VA(0x00460aba, 0x18c)
void CompressTest(void) {
    char* fromData;
    char* encoded;
    i32 H2_UNUSED(sourceCrcCheck);
    i32 H2_UNUSED(unpackedCrc);
    i32 hFile;
    i32l fileSize;
    char diffName[TEST_FILENAME_SIZE];
    i32 H2_UNUSED(sourceCrc);
    char* unpackedData;
    i32l compSize;
    i32l H2_UNUSED(plainSize);

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
    hFile = open(diffName, _O_BINARY);
    if (hFile == -1)
        FileError(diffName);
    read(hFile, fromData, fileSize);
    LogStr(const_cast<char*>("C3"));
    sourceCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
    LogStr(const_cast<char*>("C4"));
    close(hFile);
    LogStr(const_cast<char*>("C5"));
    compSize = EncodeData(encoded, fromData, fileSize);
    LogStr(const_cast<char*>("C6"));
    plainSize = DecodeData(unpackedData, encoded, compSize);
    LogStr(const_cast<char*>("C7"));
    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
    sourceCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
    H2_FREE(fromData);
    H2_FREE(encoded);
    H2_FREE(unpackedData);
    LogStr(const_cast<char*>("C8"));
}
#if H2_RETAIL_COMPILER
#undef sourceCrc
#undef sourceCrcCheck
#endif

VA(0x00460c46, 0x46)
void CompressTest3(void) {
    char buffer[TEST_MESSAGE_CAPACITY];
    i32 i;
    for (i = 0; i < COMPRESS_TEST_ITERATIONS; i++) {
        sprintf(buffer, "Test # %d", i);
        AiPrint(buffer);
        CompressTest2();
    }
}

#if H2_RETAIL_COMPILER
#define mapX col5
#define mapY row15
#endif
VA(0x00460c8c, 0x17d)
i32 game::CountShrines(i32 player) {
    town* castle;
    i32 count;
    mapCell* cell;
    i32 mapY;
    i32 mapX;
    hero* occupier;

    if (xIsExpansionMap == 0)
        return 0;
    count = 0;
    for (mapY = 0; mapY < MAP_HEIGHT; mapY++) {
        for (mapX = 0; mapX < MAP_WIDTH; mapX++) {
            cell = WORLDMAP->GetCell(mapX, mapY);
            castle = NULL;
            if (cell->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))) {
                castle = GetCastle(cell->m_objectMetadata);
            } else if (cell->m_triggerType
                       == (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))) {
                occupier = gpGame->GetHero(cell->m_objectMetadata);
                if (occupier->m_locationType
                    == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE)))
                    castle = GetCastle(occupier->m_occupiedTown);
            }
            if (castle != NULL && castle->m_owner == player
                && HAS(castle->m_buildings, IDX(TOWN_BUILDING_TAVERN))
                && castle->m_type == FACTION_NECROMANCER)
                count++;
        }
    }
    return count;
}
#if H2_RETAIL_COMPILER
#undef mapX
#undef mapY
#endif

DATA(0x004f7554) i8 giMonType[] = {
    IDX(CREATURE_PEASANT),
    IDX(CREATURE_TROLL),
    IDX(CREATURE_DWARF),
    IDX(CREATURE_ROC),
    IDX(CREATURE_OGRE),
    IDX(CREATURE_DRUID),
    IDX(CREATURE_VAMPIRE),
    IDX(CREATURE_WOLF),
    IDX(CREATURE_CENTAUR),
    IDX(CREATURE_GARGOYLE),
    IDX(CREATURE_UNICORN),
    IDX(CREATURE_LICH)
};
DATA(0x005249a0) char bMapInitialized = false;
DATA(0x005245f0) i32 iViewArmyNumTroops;
DATA(0x005245e8) i8 gbNGHeroType[GAME_PLAYER_COUNT];
DATA(0x005245f8) SMonFrameInfo sViewArmyMonFrameInfo;
DATA(0x00524960) i16 giUABaseX;
DATA(0x00524962) i16 giUABaseY;
DATA(0x00524998) b32 giEndSequence;
DATA(0x00524970) b32 gbDismissArmy;
DATA(0x0052493c) i8 gbNGHuman[GAME_PLAYER_COUNT];
DATA(0x0052496c) i32 iViewArmyFrame;
DATA(0x00524938) b32 gbAllowUpgrade;
DATA(0x0052497c) H2_ENUM_STORAGE(CreatureType, i32) iViewArmyType;
DATA(0x0052495c) class hero* viewSpellsHero;
DATA(0x005245f4) b32 gbUpgradeArmy;
DATA(0x0052494c) i16 RandMineQty[AI_RANDOM_MINE_TYPE_COUNT];
DATA(0x00524988) char gcCurMapName[GAME_CURRENT_MAP_NAME_SIZE];
DATA(0x00524964) i8 gbNGDifficulty[GAME_PLAYER_COUNT];
DATA(0x00524934) H2_ENUM_STORAGE(CreatureType, i32) iViewArmyUpgradeToType;
DATA(0x00524948) i32 viewArmyBaseX;
DATA(0x00524944) i32 viewArmyBaseY;
DATA(0x00524974) i8 gbNGColor[GAME_PLAYER_COUNT];
DATA(0x00524942) i16 giUARadius;
DATA(0x00524980) i8 gbNGPlayerPos[GAME_PLAYER_COUNT];
DATA(0x00524930) i32 viewArmyFacingWIPXMod;
