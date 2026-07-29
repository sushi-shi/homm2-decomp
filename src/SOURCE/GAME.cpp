#include <va.h>
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

#define GAME_SCORE_EXTRA_LARGE_DAY_SCALE DATA_COMPGEN(0x004eb748, dAYSCALEConstant, 0.6)
#define GAME_SCORE_LARGE_DAY_SCALE DATA_COMPGEN(0x004eb750, dAYSCALEConstant3, 0.8)
#define GAME_SCORE_SMALL_DAY_SCALE DATA_COMPGEN(0x004eb758, dAYSCALEConstant2, 1.4)
#define GAME_VIEW_ARMY_FRAME_DELAY_SCALE DATA_COMPGEN(0x004eb6f8, dELAYSCALEConstant, 1.35)

H2_ENUM_BEGIN(MapTilesetConstant)
    WAGON_CAMP_ACTIVE_FRAME = 129
H2_ENUM_END(MapTilesetConstant)

H2_ENUM_BEGIN(ExpansionCampaignSaveConstant)
    CAMPAIGN_SAVE_PREFIX_SIZE = 0x4f // persisted prefix, up to m_pad_0x4f
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
    SAVE_MARKER_SCRATCH_COUNT          = 3,
    LOAD_DIMENSION_SCRATCH_COUNT       = 11,
    LOAD_CURRENT_PLAYER_SCRATCH_SIZE   = 8,
    LOAD_MARKER_SCRATCH_SIZE           = 8,
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

H2_ENUM_BEGIN(GameNextPlayerConstant)
    NEXT_PLAYER_SCRATCH_SIZE = 20
H2_ENUM_END(GameNextPlayerConstant)

H2_ENUM_BEGIN(GameInitialStateConstant)
    INITIAL_DIFFICULTY_RATING   = 1,
    INITIAL_PLAYER_COUNT        = 4,
    INITIAL_CALENDAR_VALUE      = 1,
    INITIAL_MAP_CHANGE_SEQUENCE = 1,
    // Retail seeds both hero and town record types in nine-record groups
    // before map data is applied.
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
    WEEK_NAME_LAST                = KB_WEEK_NAME_COUNT - 2,
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
    MAP_RATING_NORMAL_BONUS = 20,
    MAP_RATING_HARD_BONUS   = 40,
    MAP_RATING_EXPERT_BONUS = 80
H2_ENUM_END(GameScoreConstant)

H2_ENUM_BEGIN(GameJoinConstant)
    JOIN_HEADER_SIZE = 2,
    JOIN_BUFFER_SIZE = 700000
H2_ENUM_END(GameJoinConstant)

H2_ENUM_BEGIN(RemoteSaveConstant)
    TRANSMIT_FILENAME_CAPACITY       = 456,
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

#define SCORE_SECOND_TIER_FACTOR DATA_COMPGEN(0x004eb760, tIERFACTORConstant2, 0.5)
#define SCORE_SECOND_TIER_BASE_DEDUCTION DATA_COMPGEN(0x004eb768, bASEDEDUCTIONConstant, 30.0)
#define SCORE_THIRD_TIER_FACTOR 0.25
#define SCORE_THIRD_TIER_BASE_DEDUCTION DATA_COMPGEN(0x004eb778, bASEDEDUCTIONConstant2, 60.0)
#define SCORE_FINAL_TIER_FACTOR DATA_COMPGEN(0x004eb780, tIERFACTORConstant, 0.125)

H2_ENUM_BEGIN(GameDiffEncoding)
    COMMAND_SHIFT      = 7,
    LEN_WORD_FLAG      = 0x40,
    LEN_BYTE_FLAG      = 0x20,
    LEN_WORD_HIGH_MASK = 0x3f,
    LEN_HIGH_MASK      = 0x2f0000,
    LEN_LOW_MASK       = 0xffff,
    LEN_SHORT_MASK     = 0x1f,
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
    CASTLE_METADATA_TOP_OFFSET    = 2,
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

inline i32 IsTownObjectTileset(TilesetId tileset) {
    return tileset >= TILESET_OBJNTOWN && tileset <= TILESET_OBJNTWRD;
}

inline MineType RandomMineType(MineType first, MineType last) {
    return static_cast<MineType>(Random(IDX(first), IDX(last)));
}

H2_ENUM_BEGIN(ArtifactGuardianConstant)
    ARTIFACT_GUARDIAN_CHOICE_COUNT = 10,
    MAJOR_GUARDIAN_CHOICE_FIRST    = 0,
    MAJOR_GUARDIAN_CHOICE_COUNT    = 6,
    MINOR_GUARDIAN_CHOICE_FIRST    = 6,
    MINOR_GUARDIAN_CHOICE_COUNT    = 4
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

DATA(0x004f70e0) b32 gbGameOver = false;
// Fixed source-file and line anchors preserve allocation diagnostics.
DATA(0x004f70e4) static i16 gSaveSourceLine = 0x294; // NOLINT(readability-magic-numbers)
DATA(0x004f71a8) static i16 gLoadSourceLine = 0x44f; // NOLINT(readability-magic-numbers)
DATA(0x004f7274) static i16 gMapSourceLine = 0xaf4; // NOLINT(readability-magic-numbers)
DATA(0x004f75c4) static i16 gTransmitSourceLine = 0x1a4e; // NOLINT(readability-magic-numbers)
DATA(0x004f77b8) static i16 gReceiveSourceLine = 0x1b2d; // NOLINT(readability-magic-numbers)
DATA(0x004f7a60) static i16 gDiffSourceLine = 0x1d66; // NOLINT(readability-magic-numbers)
DATA(0x004f7e90) static i16 gCompressTest2SourceLine = 0x1f72; // NOLINT(readability-magic-numbers)
DATA(0x004f7f84) static i16 gCompressTestSourceLine = 0x1f95; // NOLINT(readability-magic-numbers)

#define RETAIL_FILE const_cast<char*>("I:\\Projects\\Heroes\\Prog\\SOURCE\\GAME.CPP")

H2_ENUM_BEGIN(GameTuningConstant)
    RANDOM_SCAN_RETRY_LIMIT          = 10000,
    EXPERIENCE_HERO_PRESENCE_BONUS   = 500,
    MINE_FLAG_OVERWRITE_LIMIT        = 0x30,   // highest passive object id a mine flag may cover
    RANDOM_MONSTER_SPRITE_FIRST      = 0x43, // MONS32 placeholder frames for random monsters 1-4
    RANDOM_MONSTER_SPRITE_LAST       = 0x46,
    RANDOM_MONSTER_SPRITE_TO_TRIGGER =
        0x70,                     // sprite index + 0x70 == its MAP_TRIGGER_RANDOM_MONSTER_LEVEL_*
    BANK_GUARDIAN_FLAG               = 0x100,   // creature-bank metadata: defenders present
    TOWN_RECORD_TYPE_MASK            = 0x7f // saved town record: low bits carry the race
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

#define GAME_HANDICAP_MODERATE_RESOURCE_FACTOR DATA_COMPGEN(0x004eb6e8, rESOURCEFACTORConstant, 0.85)
#define GAME_HANDICAP_SEVERE_RESOURCE_FACTOR 0.7
#define GAME_HANDICAP_MODERATE_DAILY_PENALTY DATA_COMPGEN(0x004eb720, dAILYPENALTYConstant, 0.15)
#define GAME_HANDICAP_SEVERE_DAILY_PENALTY DATA_COMPGEN(0x004eb728, dAILYPENALTYConstant2, 0.30)
#define GAME_DIFFICULTY_EASY_GOLD_FACTOR DATA_COMPGEN(0x004eb700, gOLDFACTORConstant, 0.75)
#define GAME_DIFFICULTY_HARD_GOLD_FACTOR DATA_COMPGEN(0x004eb708, gOLDFACTORConstant4, 1.29)
#define GAME_DIFFICULTY_EXPERT_GOLD_FACTOR DATA_COMPGEN(0x004eb710, gOLDFACTORConstant2, 1.45)
#define GAME_DIFFICULTY_IMPOSSIBLE_GOLD_FACTOR DATA_COMPGEN(0x004eb718, gOLDFACTORConstant3, 1.6)
#define WEEKLY_HARD_GROWTH_FACTOR DATA_COMPGEN(0x004eb730, gROWTHFACTORConstant3, 1.20)
#define WEEKLY_EXPERT_GROWTH_FACTOR DATA_COMPGEN(0x004eb738, gROWTHFACTORConstant2, 1.32)
#define WEEKLY_IMPOSSIBLE_GROWTH_FACTOR DATA_COMPGEN(0x004eb740, gROWTHFACTORConstant, 1.44)

H2_ENUM_BEGIN(ViewArmyControlId)
    VIEW_ARMY_QUICK_VIEW_ID = 0x7800,
    VIEW_ARMY_UPGRADE_ID    = 0x7803
H2_ENUM_END(ViewArmyControlId)

#define WORLDMAP (&m_worldMap)

inline town* GetCastle(i32 idx) {
    return &gpGame->m_castleRecs[idx];
}
inline i8 PlayerEventByte(i8 color) {
    return gpGame->m_players[color].m_color;
}

inline b32 CanGenerateMonsterGuard(CreatureType monsterType) {
    return monsterType != CREATURE_GHOST && monsterType != CREATURE_EARTH_ELEMENTAL
           && monsterType != CREATURE_AIR_ELEMENTAL && monsterType != CREATURE_FIRE_ELEMENTAL
           && monsterType != CREATURE_WATER_ELEMENTAL;
}

inline i32 GetRandomEventSpell(SpellLevel spellLevel) {
    i32 spellMetadata =
        Random(IDX(SPELL_FIREBALL), IDX(SPELL_COUNT) - 1) + MAP_EVENT_SPELL_OFFSET;
    while (gsSpellInfo[spellMetadata - MAP_EVENT_SPELL_OFFSET].level != spellLevel) {
        spellMetadata =
            Random(IDX(SPELL_FIREBALL), IDX(SPELL_COUNT) - 1) + MAP_EVENT_SPELL_OFFSET;
    }
    return spellMetadata;
}

H2_ENUM_BEGIN(PlayerDataSerializationConstant)
    PLAYER_SAVE_SCRATCH_SIZE       = 52,
    PLAYER_SAVE_SCRATCH_CLEAR_SIZE = 48,
    PLAYER_SAVE_RESERVED_SIZE      = 42,
    PLAYER_SAVE_CHEATED_FLAG_SIZE  = 1
H2_ENUM_END(PlayerDataSerializationConstant)

VA(0x004708b0, 0x23d)
void playerData::Write(i32 file) {
    char unused[PLAYER_SAVE_SCRATCH_SIZE];

    write(file, &m_color, sizeof(m_color));
    write(file, &m_heroCount, sizeof(m_heroCount));
    write(file, &m_currentHero, sizeof(m_currentHero));
    write(file, &m_heroLocatorPage, sizeof(m_heroLocatorPage));
    write(file, m_heroIds, sizeof(m_heroIds));
    write(file, m_availableHeroIds, sizeof(m_availableHeroIds));
    // Retail clears six more scratch bytes than it persists.
    memset(unused, 0, PLAYER_SAVE_SCRATCH_CLEAR_SIZE);
    write(file, unused, PLAYER_SAVE_RESERVED_SIZE);
    write(file, &gpGame->m_cheated, PLAYER_SAVE_CHEATED_FLAG_SIZE);
    write(file, &m_cheatValue, sizeof(m_cheatValue));
    write(file, &m_aiDifficulty, sizeof(m_aiDifficulty));
    write(file, &m_minimumHeroCount, sizeof(m_minimumHeroCount));
    write(file, &m_evilInterface, sizeof(m_evilInterface));
    write(file, &m_ultimateArtifactHintChance, sizeof(m_ultimateArtifactHintChance));
    write(file, &m_ultimateArtifactHintX, sizeof(m_ultimateArtifactHintX));
    write(file, &m_ultimateArtifactHintY, sizeof(m_ultimateArtifactHintY));
    write(file, &m_daysLeft, sizeof(m_daysLeft));
    write(file, &m_townCount, sizeof(m_townCount));
    write(file, &m_currentTown, sizeof(m_currentTown));
    write(file, &m_townLocatorPage, sizeof(m_townLocatorPage));
    write(file, m_townIds, sizeof(m_townIds));
    write(file, m_resources, sizeof(m_resources));
    write(file, m_aiData.m_income, sizeof(m_aiData.m_income));
    write(file, &m_barrierTents, sizeof(m_barrierTents));
    write(file, &m_barrierTents, sizeof(m_barrierTents));
    write(file, m_unknownad, sizeof(m_unknownad));
}

VA(0x00470aed, 0x22d)
void playerData::Read(i32 file) {
    char unused[PLAYER_SAVE_SCRATCH_SIZE];

    read(file, &m_color, sizeof(m_color));
    read(file, &m_heroCount, sizeof(m_heroCount));
    read(file, &m_currentHero, sizeof(m_currentHero));
    read(file, &m_heroLocatorPage, sizeof(m_heroLocatorPage));
    read(file, m_heroIds, sizeof(m_heroIds));
    read(file, m_availableHeroIds, sizeof(m_availableHeroIds));
    read(file, unused, PLAYER_SAVE_RESERVED_SIZE);
    read(file, &gpGame->m_cheated, PLAYER_SAVE_CHEATED_FLAG_SIZE);
    read(file, &m_cheatValue, sizeof(m_cheatValue));
    read(file, &m_aiDifficulty, sizeof(m_aiDifficulty));
    read(file, &m_minimumHeroCount, sizeof(m_minimumHeroCount));
    read(file, &m_evilInterface, sizeof(m_evilInterface));
    read(file, &m_ultimateArtifactHintChance, sizeof(m_ultimateArtifactHintChance));
    read(file, &m_ultimateArtifactHintX, sizeof(m_ultimateArtifactHintX));
    read(file, &m_ultimateArtifactHintY, sizeof(m_ultimateArtifactHintY));
    read(file, &m_daysLeft, sizeof(m_daysLeft));
    read(file, &m_townCount, sizeof(m_townCount));
    read(file, &m_currentTown, sizeof(m_currentTown));
    read(file, &m_townLocatorPage, sizeof(m_townLocatorPage));
    read(file, m_townIds, sizeof(m_townIds));
    read(file, m_resources, sizeof(m_resources));
    read(file, m_aiData.m_income, sizeof(m_aiData.m_income));
    read(file, &m_barrierTents, sizeof(m_barrierTents));
    read(file, &m_barrierTents, sizeof(m_barrierTents));
    read(file, m_unknownad, sizeof(m_unknownad));
}

VA(0x00470d1a, 0x12d)
i32 playerData::NextHero(i32) {
    i32 current = -1;
    i32 i;

    if (gpCurPlayer->m_currentHero != -1) {
        for (i = 0; i < gpCurPlayer->m_heroCount; i++) {
            if (gpCurPlayer->m_heroIds[i] == gpCurPlayer->m_currentHero)
                current = i;
        }
    }

    for (i = current + 1; i < gpCurPlayer->m_heroCount; i++) {
        if (gpGame->IsMobile(gpCurPlayer->m_heroIds[i]))
            return m_heroIds[i];
    }
    for (i = 0; i < current + 1; i++) {
        if (gpGame->IsMobile(gpCurPlayer->m_heroIds[i]))
            return m_heroIds[i];
    }
    return -1;
}

VA(0x00470e47, 0x65)
i32 playerData::HasMobileHero(void) {
    i32 i;
    for (i = 0; i < m_heroCount; i++) {
        if (gpGame->IsMobile(m_heroIds[i]))
            return 1;
    }
    return 0;
}

VA(0x00470eac, 0x64)
i32 GetNumObelisks(i32 color) {
    i32 count = 0;
    i32 i;
    for (i = 0; i < GAME_BOAT_COUNT; i++) {
        if (gpGame->m_obeliskVisitors[i] & (1 << color))
            count++;
    }
    return count;
}

VA(0x00470f10, 0xca)
i32 playerData::BuildingsOwned(FactionType townType, BuildingSlotType buildingIndex, i32 buildState) {
    i32 count = 0;
    i32 i;
    for (i = 0; i < m_townCount; i++) {
        town* ownedTown = &gpGame->m_castleRecs[m_townIds[i]];
        if (buildingIndex < BUILDING_SLOT_DWELLING_FIRST || ownedTown->m_type == townType) {
            if (buildingIndex == BUILDING_SLOT_MAGE_GUILD) {
                if (ownedTown->m_buildings & IDX(TOWN_BUILDING_MAGE_GUILD)) {
                    if (ownedTown->m_buildState == buildState)
                        count++;
                }
            } else {
                if (ownedTown->m_buildings & (1 << IDX(buildingIndex)))
                    count++;
            }
        }
    }
    return count;
}

VA(0x00470fda, 0x97)
i32 playerData::NumOfGivenArtifact(ArtifactType artifact) {
    i32 count = 0;
    i32 i;
    for (i = 0; i < m_heroCount; i++) {
        i32 j;
        for (j = 0; j < HERO_ARTIFACT_SLOT_COUNT; j++) {
            if (gpGame->m_heroRecs[m_heroIds[i]].m_artifacts[j] == artifact)
                count++;
        }
    }
    return count;
}

VA(0x00471071, 0x82)
i32 game::MineTypesOwned(i32 owner, MineType mineType) {
    i32 num = 0;
    i32 i;
    for (i = 0; i < GAME_MINE_COUNT; i++) {
        if (m_mines[i].owner == owner && m_mines[i].resourceType == mineType)
            num++;
    }
    return num;
}

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

VA(0x004710f3, 0x40d)
void ComputeUALoc(i32 player) {
    i32 result = gpGame->SetupPuzzlePieces(player, 1);
    if (result < MINIMUM_PUZZLE_PIECES
        || gpGame->m_ultimateArtifactId == ARTIFACT_NONE) {
        gpGame->m_players[player].m_ultimateArtifactHintChance = 0;
        gpGame->m_players[player].m_ultimateArtifactHintX = HINT_COORDINATE_UNKNOWN;
        gpGame->m_players[player].m_ultimateArtifactHintY = HINT_COORDINATE_UNKNOWN;
    } else {
        i32 probability =
            (result - MINIMUM_PUZZLE_PIECES) * HINT_CHANCE_PER_PIECE;
        if (probability > HINT_CHANCE_MAXIMUM)
            probability = HINT_CHANCE_MAXIMUM;
        if (probability < HINT_CHANCE_MINIMUM)
            probability = HINT_CHANCE_MINIMUM;
        gpGame->m_players[player].m_ultimateArtifactHintChance = static_cast<i8>(probability);

        if (Random(HINT_CHANCE_MINIMUM, HINT_CHANCE_MAXIMUM)
            <= gpGame->m_players[player].m_ultimateArtifactHintChance) {
            gpGame->m_players[player].m_ultimateArtifactHintX = gpGame->m_ultimateArtifactX;
            gpGame->m_players[player].m_ultimateArtifactHintY = gpGame->m_ultimateArtifactY;
        } else {
            i32 x = HINT_COORDINATE_UNKNOWN;
            i32 y = HINT_COORDINATE_UNKNOWN;
            i32 direction = 0;
            i32 tries = 0;
            while (
                !(x >= 0 && (&x)[0] < MAP_WIDTH && y >= 0 && (&y)[0] < MAP_HEIGHT
                  && gpGame->m_worldMap.Row(y)[x].m_triggerType == MAP_OBJECT_NONE
                  && gpGame->m_worldMap.Row(y)[x].m_objectIndex == MAPCELL_SPRITE_NONE
                  && gpGame->m_worldMap.Row(y)[x].m_overlayIndex == MAPCELL_SPRITE_NONE
                  && giGroundToTerrain[gpGame->m_worldMap.Row(y)[x].m_terrainImageIndex]
                         != TERRAIN_WATER)
            ) {
                tries++;
                direction = 0;
                while (direction == 0)
                    direction = HINT_OFFSET_CENTER - Random(0, HINT_OFFSET_ROLL_MAXIMUM)
                                - Random(0, HINT_OFFSET_ROLL_MAXIMUM)
                                - Random(0, HINT_OFFSET_ROLL_MAXIMUM);
                x = gpGame->m_ultimateArtifactX + direction;
                direction = 0;
                while (direction == 0)
                    direction = HINT_OFFSET_CENTER - Random(0, HINT_OFFSET_ROLL_MAXIMUM)
                                - Random(0, HINT_OFFSET_ROLL_MAXIMUM)
                                - Random(0, HINT_OFFSET_ROLL_MAXIMUM);
                y = gpGame->m_ultimateArtifactY + direction;
                if (tries >= HINT_LOCATION_RETRY_LIMIT) {
                    x = gpGame->m_ultimateArtifactX;
                    y = gpGame->m_ultimateArtifactY;
                    goto saveLocation;
                }
            }
        saveLocation:
            gpGame->m_players[player].m_ultimateArtifactHintX = static_cast<i8>(x);
            gpGame->m_players[player].m_ultimateArtifactHintY = static_cast<i8>(y);
        }
    }
}

H2_ENUM_BEGIN(PuzzleSetupConstant)
    PUZZLE_INTERPOLATION_TERM_COUNT = 2,
    PUZZLE_INTERFACE_SEED_STRIDE    = 3,
    PUZZLE_RANDOM_STEP_MAXIMUM      = 5,
    PUZZLE_FALLBACK_RETRY_LIMIT     = 100
H2_ENUM_END(PuzzleSetupConstant)

VA(0x00471500, 0x2ac)
i32 game::SetupPuzzlePieces(i32 player, i32 justCount) {
    i32 pieceCountTotal = GetNumObelisks(player);
    i32 unvisitedObelisks = PUZZLE_PIECE_COUNT - m_obeliskCount;
    float ratio = static_cast<float>(GetNumObelisks(player)) / m_obeliskCount;
    float interpolation =
        (ratio * ratio + ratio)
        / DATA_COMPGEN(
            0x004eb6e0,
            puzzleInterpolationTermCount,
            static_cast<float>(PUZZLE_INTERPOLATION_TERM_COUNT)
        );
    pieceCountTotal = static_cast<i32>(pieceCountTotal + unvisitedObelisks * interpolation);

    if (GetNumObelisks(player) == m_obeliskCount)
        pieceCountTotal = PUZZLE_PIECE_COUNT;
    pieceCountTotal += m_players[player].m_cheatValue;
    if (pieceCountTotal > PUZZLE_PIECE_COUNT)
        pieceCountTotal = PUZZLE_PIECE_COUNT;
    if (justCount)
        return pieceCountTotal;

    memset(puzzlePiecesRemoved, 0, PUZZLE_PIECE_STORAGE_SIZE);
    SRand(
        m_players[player].m_color
        + m_players[player].m_evilInterface * PUZZLE_INTERFACE_SEED_STRIDE
    );
    i32 tries;
    i32 fallbackNum;
    i32 pieceValue;
    i32 i;
    for (i = 0; (&i)[0] < pieceCountTotal; i++) {
        for (pieceValue = 0;
             pieceValue < PUZZLE_PIECE_COUNT;
             pieceValue += SRandom(1, PUZZLE_RANDOM_STEP_MAXIMUM)) {
            if (!BitTest(puzzlePiecesRemoved, pieceValue))
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
        if (pieceValue < PUZZLE_PIECE_COUNT)
            BitSet(puzzlePiecesRemoved, pieceValue);
        else
            BitSet(puzzlePiecesRemoved, fallbackNum);
    }
    return pieceCountTotal;
}

VA(0x004717ac, 0xb5)
i32 game::IsMobile(i32 heroId) {
    if (heroId == -1)
        return 0;
    hero* mobileHero = &m_heroRecs[heroId];
    mapCell* cp = gpAdvManager->GetCell(mobileHero->m_x, mobileHero->m_y);
    return CalcTerrainCost(
               giGroundToTerrain[cp->m_terrainImageIndex],
               1,
               mobileHero->m_remainingMobility,
               IDX(mobileHero->m_secondarySkills[IDX(HERO_SKILL_PATHFINDING)]),
               cp->m_isRoad,
               0
           )
           <= mobileHero->m_remainingMobility;
}

VA(0x00471861, 0x1e)
fullMap* game::GetWorldMapData(void) {
    return &m_worldMap;
}

VA(0x0047187f, 0x11e)
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
        mapCell* cell = WORLDMAP->Row(y) + x;
        boat->savedTriggerType = cell->m_triggerType;
        boat->savedEventData = static_cast<u8>(cell->m_objectMetadata);
        cell->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT;
        cell->m_objectMetadata = boatIdx;
    }
    return boatIdx;
}

VA(0x0047199d, 0x5a)
i32 game::Scan(i8* array, i32 start, i32 length) {
    i32 i;
    for (i = start; i < length + start; i++) {
        if (array[i] == -1)
            return i;
    }
    return -1;
}

VA(0x004719f7, 0x76)
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

VA(0x00471a6d, 0x213)
i32 game::GetNewHeroId(i32, FactionType heroClass, i32 requireExperienced) {
    i32 result = -1;
    i32 previousHero;
    i32 heroId = -1;
    i32 attempts = 0;
    i32 oldHeroId;
    while (attempts < HERO_SELECTION_RETRY_LIMIT) {
        attempts++;
        heroId = Random(0, IDX(GAME_HERO_COUNT) - 1);
        if (m_availableHeroes[heroId] != HERO_AVAILABILITY_UNAVAILABLE
            && m_availableHeroes[heroId] != WEEKLY_AVAILABLE_HERO)
            continue;
        if (m_availableHeroes[heroId] == WEEKLY_AVAILABLE_HERO
            && attempts < HERO_SELECTION_REUSE_RETRY_LIMIT)
            continue;
        if (heroClass >= FACTION_KNIGHT && heroClass <= FACTION_NECROMANCER
            && attempts < HERO_SELECTION_FACTION_RETRY_LIMIT
            && m_heroRecs[heroId].m_cursorType != heroClass)
            continue;
        if (requireExperienced && attempts < HERO_SELECTION_EXPERIENCE_RETRY_LIMIT
            && m_heroRecs[heroId].m_experience < HERO_SELECTION_MINIMUM_EXPERIENCE
            && (m_heroRecs[heroId].m_artifacts[0] == ARTIFACT_NONE
                || m_heroRecs[heroId].m_artifacts[0] == ARTIFACT_MAGIC_BOOK)
            && (m_heroRecs[heroId].m_artifacts[1] == ARTIFACT_NONE
                || m_heroRecs[heroId].m_artifacts[1] == ARTIFACT_MAGIC_BOOK))
            continue;
        if (gbInCampaign && attempts < HERO_SELECTION_CAMPAIGN_RETRY_LIMIT
            && m_heroRecs[heroId].m_portrait >= CAMPAIGN_HERO_ROLAND
            && m_heroRecs[heroId].m_portrait <= CAMPAIGN_HERO_BRAX)
            continue;
        break;
    }
    return heroId;
}

VA(0x00471c80, 0x85)
i32 game::GetTownId(i32 col, i32 row) {
    i32 i;
    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        if (m_castleRecs[i].m_x == col && m_castleRecs[i].m_y == row)
            return i;
    }
    return -1;
}

VA(0x00471d05, 0x84)
i32 game::GetMineId(i32 col, i32 row) {
    i32 i;
    for (i = 0; i < GAME_MINE_COUNT; i++) {
        if (m_mines[i].x == col && m_mines[i].y == row)
            return i;
    }
    return -1;
}

VA(0x00471d89, 0x12e)
void GenerateStandardFileName(char* source, char* destination) {
    char* extension = FindLastToken(source, '.');
    if (extension == NULL) {
        strcpy(destination, source);
        return;
    }

    *extension = '\0';
    i32 indexOut = 0;
    i32 sourceLength = strlen(source);
    i32 i;
    char c;
    for (i = 0; i < (&sourceLength)[0]; i++) {
        c = source[i];
        if (c >= 'a' && c <= 'z')
            c -= 'a' - 'A';
        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
            destination[indexOut] = c;
            indexOut++;
        }
        if (indexOut >= STANDARD_FILENAME_BASENAME_SIZE)
            i = 999; // NOLINT(readability-magic-numbers)
    }
    *extension = '.';
    strcpy(destination + indexOut, extension);
}

VA(0x00471eb7, 0xbc4)
i32 game::SaveGame(char* filename, i32 generateName, i8 expansionFormat) {
    void* emptyPayload = H2_ALLOC_AT(GAME_SAVE_BUFFER_SIZE, DATA_COMPGEN(0x004f70e8, saveGameSourceFile, RETAIL_FILE), gSaveSourceLine + 10);
    memset(emptyPayload, 0, GAME_SAVE_BUFFER_SIZE);
    if (!xIsExpansionMap)
        expansionFormat = 1;
    gpAdvManager->DemobilizeCurrHero();

    char savePathValue[SAVE_PATH_CAPACITY];
    char generatedNameStorage[SAVE_PATH_CAPACITY];
    i32 humanPlayersVar;
    i32 indexFile;
    i32 unusedTemp;
    i32 oldFlag;
    i32 saveValue;
    i32 filePadding;
    i32 compatibilityReserved;
    if (generateName) {
        if (gbInCampaign) {
            sprintf(generatedNameStorage, DATA_COMPGEN(0x004f7114, saveGameSS, "%s.%s"), filename, DATA_COMPGEN(0x004f7110, saveGameGMC, "GMC"));
        } else if (xIsPlayingExpansionCampaign) {
            sprintf(generatedNameStorage, DATA_COMPGEN(0x004f7120, saveGameSS2, "%s.%s"), filename, DATA_COMPGEN(0x004f711c, saveGameGXC, "GXC"));
        } else {
            humanPlayersVar = 0;
            for (indexFile = 0; indexFile < GAME_PLAYER_COUNT; indexFile++) {
                if (m_playerDead[indexFile] == 0 && gbHumanPlayer[indexFile])
                    humanPlayersVar++;
            }
            if (xIsExpansionMap && !expansionFormat)
                sprintf(generatedNameStorage, DATA_COMPGEN(0x004f7128, saveGameSGXD, "%s.GX%d"), filename, humanPlayersVar);
            else
                sprintf(generatedNameStorage, DATA_COMPGEN(0x004f7130, saveGameSGMD, "%s.GM%d"), filename, humanPlayersVar);
        }
    } else {
        sprintf(generatedNameStorage, filename);
    }

    if (strnicmp(generatedNameStorage, DATA_COMPGEN(0x004f7138, saveGameRMT, "RMT"), sizeof("RMT") - 1) == 0) {
        sprintf(savePathValue, DATA_COMPGEN(0x004f7144, saveGameSS3, "%s%s"), DATA_COMPGEN(0x004f713c, saveGameDATA, ".\\DATA\\"), generatedNameStorage);
    } else {
        sprintf(savePathValue, DATA_COMPGEN(0x004f714c, saveGameSS4, "%s%s"), gcGamePath, generatedNameStorage);
        if (strnicmp(generatedNameStorage, DATA_COMPGEN(0x004f7154, saveGameAUTOSAVE, "AUTOSAVE"), sizeof("AUTOSAVE") - 1) != 0
            && strnicmp(generatedNameStorage, DATA_COMPGEN(0x004f7160, saveGamePLYREXIT, "PLYREXIT"), sizeof("PLYREXIT") - 1) != 0)
            strcpy(gpGame->m_saveName, filename);
    }

    i32 handle = open(savePathValue, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (handle == -1)
        FileError(savePathValue);

    i32 legacyMarkerTemp = -1;
    if (!expansionFormat)
        write(handle, &legacyMarkerTemp, sizeof(legacyMarkerTemp));
    write(handle, &m_worldMap.width, sizeof(m_worldMap.width));
    write(handle, &m_worldMap.height, sizeof(m_worldMap.height));
    write(handle, &m_mapHeader, sizeof(m_mapHeader));
    write(handle, m_setupPlayerColor, CAMPAIGN_SETUP_RESET_SIZE);
    write(handle, &gbIAmGreatest, SAVE_TRUNCATED_SCALAR_SIZE);
    write(handle, this, sizeof(m_difficultyRating));
    write(handle, &giMonthType, SAVE_TRUNCATED_SCALAR_SIZE);
    write(handle, &giMonthTypeExtra, SAVE_TRUNCATED_SCALAR_SIZE);
    write(handle, &giWeekType, SAVE_TRUNCATED_SCALAR_SIZE);
    write(handle, &giWeekTypeExtra, SAVE_TRUNCATED_SCALAR_SIZE);
    write(handle, cPlayerNames, sizeof(cPlayerNames));

    char legacyData[SAVE_LEGACY_SCRATCH_SIZE];
    memset(legacyData, 0, SAVE_LEGACY_CLEAR_SIZE);
    write(handle, legacyData, SAVE_LEGACY_SERIALIZED_SIZE);
    if (xIsPlayingExpansionCampaign) {
        i32 campaignTypeInfo = SAVE_EXPANSION_CAMPAIGN_FORMAT_TAG;
        write(handle, &campaignTypeInfo, sizeof(campaignTypeInfo));
        write(handle, &xCampaign, CAMPAIGN_SAVE_PREFIX_SIZE);
    } else {
        write(handle, &gbInCampaign, sizeof(gbInCampaign));
        if (gbInCampaign)
            write(handle, &m_campaignType, CAMPAIGN_STATE_RESET_SIZE);
    }
    if (!expansionFormat)
        write(handle, &xIsExpansionMap, sizeof(xIsExpansionMap));

    gpAdvManager->PurgeMapChangeQueue();
    write(handle, &giMapChangeCtr, sizeof(giMapChangeCtr));
    GenerateStandardFileName(m_saveName, legacyData);
    write(handle, legacyData, SAVE_STANDARD_FILENAME_SIZE);
    write(handle, &m_playerCount, sizeof(m_playerCount));
    char currentPlayerInfo[SAVE_CURRENT_PLAYER_SCRATCH_SIZE];
    currentPlayerInfo[0] = static_cast<char>(giCurPlayer);
    write(handle, currentPlayerInfo, sizeof(currentPlayerInfo[0]));
    write(handle, &m_deadPlayerCount, sizeof(m_deadPlayerCount));
    write(handle, m_playerDead, sizeof(m_playerDead));

    char humanFlagsLocal[SAVE_PLAYER_FLAGS_SCRATCH_SIZE];
    for (indexFile = 0; indexFile < GAME_PLAYER_COUNT; indexFile++) {
        humanFlagsLocal[indexFile] = static_cast<char>(gbHumanPlayer[indexFile]);
        if (m_playerDead[indexFile] != 0)
            humanFlagsLocal[indexFile] = 0;
    }
    write(handle, humanFlagsLocal, GAME_PLAYER_COUNT);
    write(handle, &m_day, sizeof(m_day));
    write(handle, &m_week, sizeof(m_week));
    write(handle, &m_month, sizeof(m_month));
    for (indexFile = 0; indexFile < GAME_PLAYER_COUNT; indexFile++)
        m_players[indexFile].Write(handle);

    write(handle, &m_obeliskCount, sizeof(m_obeliskCount));
    for (indexFile = 0; indexFile < GAME_HERO_COUNT; indexFile++)
        m_heroRecs[indexFile].Write(handle, !expansionFormat);
    write(handle, m_availableHeroes, sizeof(m_availableHeroes));
    write(handle, m_castleRecs, sizeof(m_castleRecs));
    write(handle, m_castleOwners, sizeof(m_castleOwners));
    write(handle, m_dailyEventFlags, sizeof(m_dailyEventFlags));
    write(handle, m_mines, sizeof(m_mines));
    write(handle, m_mineOwners, sizeof(m_mineOwners));
    if (!expansionFormat)
        write(handle, m_randomArtifacts, IDX(ARTIFACT_COUNT));
    else
        write(handle, m_randomArtifacts, ARTIFACT_BASE_TABLE_SIZE);
    write(handle, m_boats, sizeof(m_boats));
    write(handle, m_boatSlots, sizeof(m_boatSlots));
    write(handle, m_obeliskVisitors, sizeof(m_obeliskVisitors));
    write(handle, &m_ultimateArtifactX, sizeof(m_ultimateArtifactX));
    write(handle, &m_ultimateArtifactY, sizeof(m_ultimateArtifactY));
    write(handle, &m_ultimateArtifactId, sizeof(m_ultimateArtifactId));
    write(handle, m_rumour, sizeof(m_rumour));
    write(handle, m_defaultPlayerNames, sizeof(m_defaultPlayerNames));
    // Each legacy event header persists the count and the first index word.
    write(handle, &m_rumourEventCount, SAVE_EVENT_HEADER_SIZE);
    write(
        handle,
        m_rumourEventIndices,
        m_rumourEventCount * sizeof(m_rumourEventIndices[0])
    );
    write(handle, &m_timeEventCount, SAVE_EVENT_HEADER_SIZE);
    write(handle, m_timeEventIndices, m_timeEventCount * sizeof(m_timeEventIndices[0]));
    write(handle, &m_mapEventCount, SAVE_EVENT_HEADER_SIZE);
    write(handle, m_mapEventIndices, m_mapEventCount * sizeof(m_mapEventIndices[0]));

    i32 markerBuffer[SAVE_MARKER_SCRATCH_COUNT];
    markerBuffer[0] = GAME_FILE_MARKER;
    i32 unusedMarkerInfo = GAME_UNUSED_FILE_MARKER;
    write(handle, markerBuffer, sizeof(markerBuffer[0]));
    write(handle, &iMaxMapExtra, sizeof(iMaxMapExtra));
    write(handle, markerBuffer, sizeof(markerBuffer[0]));
    for (indexFile = 1; indexFile < iMaxMapExtra; indexFile++) {
        write(handle, markerBuffer, sizeof(markerBuffer[0]));
        write(handle, pwSizeOfMapExtra + indexFile, sizeof(pwSizeOfMapExtra[indexFile]));
        if (ppMapExtra[indexFile] != NULL)
            write(handle, ppMapExtra[indexFile], pwSizeOfMapExtra[indexFile]);
        else
            write(handle, emptyPayload, pwSizeOfMapExtra[indexFile]);
    }
    write(handle, markerBuffer, sizeof(markerBuffer[0]));
    write(handle, mapExtra, MAP_WIDTH * MAP_HEIGHT);
    write(handle, markerBuffer, sizeof(markerBuffer[0]));
    m_worldMap.Write(handle);
    write(handle, markerBuffer, sizeof(markerBuffer[0]));
    close(handle);
    H2_FREE_AT(emptyPayload, DATA_COMPGEN(0x004f716c, saveGameSourceFile2, RETAIL_FILE), gSaveSourceLine + 0xed);
    return 1;
}

VA(0x00472a7b, 0xb44)
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
    strcpy(m_saveName, DATA_COMPGEN(0x004f7194, setupOrigDataNEWGAME, "NEWGAME"));
    m_playerCount = INITIAL_PLAYER_COUNT;
    m_deadPlayerCount = 0;
    memset(m_playerDead, 0, sizeof(m_playerDead));
    m_month = INITIAL_CALENDAR_VALUE;
    m_week = m_month;
    m_day = m_week;
    giCurTurn = INITIAL_CALENDAR_VALUE;

    i32 i;
    i32 j;
    for (i = 0; i < GAME_PLAYER_COUNT; i++) {
        strcpy(m_defaultPlayerNames + i * GAME_DEFAULT_PLAYER_NAME_SIZE, DATA_COMPGEN(0x004f719c, setupOrigDataEmptyString, ""));
        if (i < (&giNumHumanPlayers)[0]) {
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
        memset(m_heroRecs[i].m_artifacts, IDX(ARTIFACT_NONE), sizeof(m_heroRecs[i].m_artifacts));
        m_heroRecs[i].m_patrolY = HERO_DESTINATION_NONE;
        m_heroRecs[i].m_patrolX = m_heroRecs[i].m_patrolY;
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
        m_heroRecs[i].m_destinationX = m_heroRecs[i].m_destinationY;
        m_heroRecs[i].m_level = HERO_INITIAL_LEVEL;
        m_heroRecs[i].m_spellPoints =
            m_heroRecs[i].Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
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
    m_ultimateArtifactX = m_ultimateArtifactY;
    memset(m_obeliskVisitors, 0, sizeof(m_obeliskVisitors));
    strcpy(gpGame->m_saveName, DATA_COMPGEN(0x004f71a0, setupOrigDataNEWGAME2, "NEWGAME"));
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

VA(0x004735bf, 0xc27)
void game::LoadGame(char* filename, i32 loadFromFile, i32) {
    LogStr(DATA_COMPGEN(0x004f71ac, loadGameLG1, "LG1"));
    if (loadFromFile) {
        SetupOrigData();
        return;
    }
    LogStr(DATA_COMPGEN(0x004f71b0, loadGameLG2, "LG2"));
    i32 humansLoaded3 = 0;
    gbGameOver = false;
    m_gameLoaded = 1;

    char path28[SAVE_PATH_CAPACITY];
    if (loadFromFile || strnicmp(filename, DATA_COMPGEN(0x004f71b4, loadGameRMT, "RMT"), sizeof(DATA_COMPGEN(0x004f71d0, loadGameRMT2, "RMT")) - 1) == 0)
        sprintf(path28, DATA_COMPGEN(0x004f71c0, loadGameSS, "%s%s"), DATA_COMPGEN(0x004f71b8, loadGameDATA, ".\\DATA\\"), filename);
    else
        sprintf(path28, DATA_COMPGEN(0x004f71c8, loadGameSS2, "%s%s"), gcGamePath, filename);

    i32 file0 = open(path28, _O_BINARY);
    if (file0 == -1)
        FileError(path28);
    ClearMapExtra();

    i8 expansionMarker0 = 0;
    i32 width8;
    i32 height9[LOAD_DIMENSION_SCRATCH_COUNT];
    read(file0, &width8, sizeof(width8));
    if (width8 == -1) {
        expansionMarker0 = 1;
        read(file0, &width8, sizeof(width8));
    }
    read(file0, height9, sizeof(height9[0]));
    SetMapSize(width8, height9[0]);
    read(file0, &m_mapHeader, sizeof(m_mapHeader));
    read(file0, m_setupPlayerColor, CAMPAIGN_SETUP_RESET_SIZE);
    read(file0, &gbIAmGreatest, SAVE_TRUNCATED_SCALAR_SIZE);
    read(file0, this, sizeof(m_difficultyRating));
    read(file0, &giMonthType, SAVE_TRUNCATED_SCALAR_SIZE);
    read(file0, &giMonthTypeExtra, SAVE_TRUNCATED_SCALAR_SIZE);
    read(file0, &giWeekType, SAVE_TRUNCATED_SCALAR_SIZE);
    read(file0, &giWeekTypeExtra, SAVE_TRUNCATED_SCALAR_SIZE);
    read(file0, cPlayerNames, sizeof(cPlayerNames));

    char oldData3[SAVE_LEGACY_CLEAR_SIZE];
    read(file0, oldData3, SAVE_LEGACY_SERIALIZED_SIZE);
    read(file0, &gbInCampaign, sizeof(gbInCampaign));
    if (gbInCampaign == 1) {
        read(file0, &m_campaignType, CAMPAIGN_STATE_RESET_SIZE);
    } else if (gbInCampaign == SAVE_EXPANSION_CAMPAIGN_FORMAT_TAG) {
        xIsPlayingExpansionCampaign = 1;
        gbInCampaign = false;
        read(file0, &xCampaign, CAMPAIGN_SAVE_PREFIX_SIZE);
    }
    if (expansionMarker0)
        read(file0, &xIsExpansionMap, sizeof(xIsExpansionMap));

    gpAdvManager->PurgeMapChangeQueue();
    read(file0, &giMapChangeCtr, sizeof(giMapChangeCtr));
    read(file0, oldData3, SAVE_STANDARD_FILENAME_SIZE);
    if (strnicmp(filename, "RMT", sizeof("RMT") - 1) != 0)
        sprintf(gpGame->m_saveName, filename);
    read(file0, &m_playerCount, sizeof(m_playerCount));

    char currentPlayer6[LOAD_CURRENT_PLAYER_SCRATCH_SIZE];
    read(file0, currentPlayer6, sizeof(currentPlayer6[0]));
    giCurPlayer = currentPlayer6[0];
    read(file0, &m_deadPlayerCount, sizeof(m_deadPlayerCount));
    read(file0, m_playerDead, sizeof(m_playerDead));

    char humanFlags1[SAVE_PLAYER_FLAGS_SCRATCH_SIZE];
    read(file0, humanFlags1, GAME_PLAYER_COUNT);
    i32 i29;
    for (i29 = 0; i29 < GAME_PLAYER_COUNT; i29++) {
        if (humanFlags1[i29] && humansLoaded3 < (&giNumHumanPlayers)[0]) {
            humansLoaded3++;
            gbHumanPlayer[i29] = 1;
        } else {
            gbHumanPlayer[i29] = 0;
        }
    }
    for (i29 = 0; i29 < GAME_PLAYER_COUNT; i29++) {
        if (gbHumanPlayer[i29]) {
            if (!gbRemoteOn || (&i29)[0] == giThisGamePos)
                gbThisNetHumanPlayer[i29] = 1;
            else
                gbThisNetHumanPlayer[i29] = 0;
        } else {
            gbThisNetHumanPlayer[i29] = 0;
        }
    }

    read(file0, &m_day, sizeof(m_day));
    read(file0, &m_week, sizeof(m_week));
    read(file0, &m_month, sizeof(m_month));
    giCurTurn = (m_week - 1) * EVENT_DAYS_PER_WEEK
                + (m_month - 1) * EVENT_DAYS_PER_MONTH + m_day;
    for (i29 = 0; i29 < GAME_PLAYER_COUNT; i29++)
        m_players[i29].Read(file0);

    read(file0, &m_obeliskCount, sizeof(m_obeliskCount));
    for (i29 = 0; i29 < GAME_HERO_COUNT; i29++)
        m_heroRecs[i29].Read(file0, expansionMarker0);
    read(file0, m_availableHeroes, sizeof(m_availableHeroes));
    read(file0, m_castleRecs, sizeof(m_castleRecs));
    read(file0, m_castleOwners, sizeof(m_castleOwners));
    read(file0, m_dailyEventFlags, sizeof(m_dailyEventFlags));
    read(file0, m_mines, sizeof(m_mines));
    read(file0, m_mineOwners, sizeof(m_mineOwners));
    if (expansionMarker0)
        read(file0, m_randomArtifacts, IDX(ARTIFACT_COUNT));
    else
        read(file0, m_randomArtifacts, ARTIFACT_BASE_TABLE_SIZE);
    read(file0, m_boats, sizeof(m_boats));
    read(file0, m_boatSlots, sizeof(m_boatSlots));
    read(file0, m_obeliskVisitors, sizeof(m_obeliskVisitors));
    read(file0, &m_ultimateArtifactX, sizeof(m_ultimateArtifactX));
    read(file0, &m_ultimateArtifactY, sizeof(m_ultimateArtifactY));
    read(file0, &m_ultimateArtifactId, sizeof(m_ultimateArtifactId));
    read(file0, m_rumour, sizeof(m_rumour));
    read(file0, m_defaultPlayerNames, sizeof(m_defaultPlayerNames));
    read(file0, &m_rumourEventCount, SAVE_EVENT_HEADER_SIZE);
    read(
        file0,
        m_rumourEventIndices,
        m_rumourEventCount * sizeof(m_rumourEventIndices[0])
    );
    read(file0, &m_timeEventCount, SAVE_EVENT_HEADER_SIZE);
    read(file0, m_timeEventIndices, m_timeEventCount * sizeof(m_timeEventIndices[0]));
    read(file0, &m_mapEventCount, SAVE_EVENT_HEADER_SIZE);
    read(file0, m_mapEventIndices, m_mapEventCount * sizeof(m_mapEventIndices[0]));

    char marker0[LOAD_MARKER_SCRATCH_SIZE];
    read(file0, marker0, sizeof(i32));
    read(file0, &iMaxMapExtra, sizeof(iMaxMapExtra));
    read(file0, marker0, sizeof(i32));
    ppMapExtra = reinterpret_cast<void**>(
        H2_ALLOC_AT(iMaxMapExtra * sizeof(*ppMapExtra), DATA_COMPGEN(0x004f71d4, loadGameSourceFile, RETAIL_FILE), gLoadSourceLine + 0xcb)
    );
    pwSizeOfMapExtra = reinterpret_cast<i16*>(
        H2_ALLOC_AT(iMaxMapExtra * sizeof(*pwSizeOfMapExtra), DATA_COMPGEN(0x004f71fc, loadGameSourceFile2, RETAIL_FILE), gLoadSourceLine + 0xcc)
    );
    memset(ppMapExtra, 0, iMaxMapExtra * sizeof(*ppMapExtra));
    memset(pwSizeOfMapExtra, 0, iMaxMapExtra * sizeof(*pwSizeOfMapExtra));
    for (i29 = 1; (&i29)[0] < iMaxMapExtra; i29++) {
        read(file0, marker0, sizeof(i32));
        read(file0, pwSizeOfMapExtra + i29, sizeof(pwSizeOfMapExtra[i29]));
        ppMapExtra[i29] = H2_ALLOC_AT(pwSizeOfMapExtra[i29], DATA_COMPGEN(0x004f7224, loadGameSourceFile3, RETAIL_FILE), gLoadSourceLine + 0xd5);
        read(file0, ppMapExtra[i29], pwSizeOfMapExtra[i29]);
    }
    read(file0, marker0, sizeof(i32));
    read(file0, mapExtra, MAP_WIDTH * MAP_HEIGHT);
    read(file0, marker0, sizeof(i32));
    m_worldMap.Read(file0, 0);
    read(file0, marker0, sizeof(i32));
    close(file0);

    gpAdvManager->m_heroContextLocked = 0;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);
    giCurWatchPlayer = giCurPlayer;
    while (!gbThisNetHumanPlayer[giCurWatchPlayer])
        giCurWatchPlayer = (giCurWatchPlayer + 1) % m_playerCount;
    giCurWatchPlayerBit = static_cast<u8>(1 << giCurWatchPlayer);
    bShowIt = gbThisNetHumanPlayer[giCurPlayer];
    SetupAdjacentMons();
    LogStr(DATA_COMPGEN(0x004f724c, loadGameLG3, "LG3"));
    gpAdvManager->CheckSetEvilInterface(0, -1);
}

VA(0x004741e6, 0x3ee)
void game::GiveTroopsToNeutralTown(i32 townId) {
    i32 unused28;
    i32 tierKey36;
    CreatureType creature28;
    i32 reinforcementRandom;
    i32 count1;
    i32 turnRollRange10;

    if ((m_castleRecs[townId].m_x > 0 || m_castleRecs[townId].m_y > 0)
        && m_castleRecs[townId].m_owner < 0) {
        reinforcementRandom = Random(REINFORCEMENT_ROLL_MIN, REINFORCEMENT_ROLL_MAX);
        turnRollRange10 = giCurTurn / REINFORCEMENT_TURN_ROLL_DIVISOR;
        if (turnRollRange10 != 0)
            reinforcementRandom += Random(0, turnRollRange10);

        if (reinforcementRandom <= REINFORCEMENT_TIER_ONE_THRESHOLD) {
            tierKey36 = REINFORCEMENT_TIER_ONE_KEY;
            count1 = Random(REINFORCEMENT_TIER_ONE_COUNT_MIN, REINFORCEMENT_TIER_ONE_COUNT_MAX);
        } else if (reinforcementRandom <= REINFORCEMENT_TIER_TWO_THRESHOLD) {
            tierKey36 = REINFORCEMENT_TIER_TWO_KEY;
            count1 = Random(REINFORCEMENT_TIER_TWO_COUNT_MIN, REINFORCEMENT_TIER_TWO_COUNT_MAX);
        } else if (reinforcementRandom <= REINFORCEMENT_TIER_THREE_THRESHOLD) {
            tierKey36 = REINFORCEMENT_TIER_THREE_KEY;
            count1 = Random(REINFORCEMENT_TIER_THREE_COUNT_MIN, REINFORCEMENT_TIER_THREE_COUNT_MAX);
        } else if (reinforcementRandom <= REINFORCEMENT_TIER_FOUR_THRESHOLD) {
            tierKey36 = REINFORCEMENT_TIER_FOUR_KEY;
            count1 = Random(REINFORCEMENT_TIER_FOUR_COUNT_MIN, REINFORCEMENT_TIER_FOUR_COUNT_MAX);
        } else {
            tierKey36 = REINFORCEMENT_TIER_FIVE_KEY;
            count1 = REINFORCEMENT_TIER_FIVE_COUNT;
        }

        count1 += giCurTurn / REINFORCEMENT_TURN_COUNT_DIVISOR;
        switch (IDX(m_castleRecs[townId].m_type) + tierKey36) {
            case REINFORCEMENT_TIER_ONE_KEY + IDX(FACTION_KNIGHT):
                creature28 = CREATURE_PEASANT;
                break;
            case REINFORCEMENT_TIER_TWO_KEY + IDX(FACTION_KNIGHT):
                creature28 = CREATURE_ARCHER;
                break;
            case REINFORCEMENT_TIER_THREE_KEY + IDX(FACTION_KNIGHT):
                creature28 = CREATURE_PIKEMAN;
                break;
            case REINFORCEMENT_TIER_FOUR_KEY + IDX(FACTION_KNIGHT):
                creature28 = CREATURE_SWORDSMAN;
                break;
            case REINFORCEMENT_TIER_FIVE_KEY + IDX(FACTION_KNIGHT):
                creature28 = CREATURE_CAVALRY;
                break;
            case REINFORCEMENT_TIER_ONE_KEY + IDX(FACTION_BARBARIAN):
                creature28 = CREATURE_GOBLIN;
                break;
            case REINFORCEMENT_TIER_TWO_KEY + IDX(FACTION_BARBARIAN):
                creature28 = CREATURE_ORC;
                break;
            case REINFORCEMENT_TIER_THREE_KEY + IDX(FACTION_BARBARIAN):
                creature28 = CREATURE_WOLF;
                break;
            case REINFORCEMENT_TIER_FOUR_KEY + IDX(FACTION_BARBARIAN):
                creature28 = CREATURE_OGRE;
                break;
            case REINFORCEMENT_TIER_FIVE_KEY + IDX(FACTION_BARBARIAN):
                creature28 = CREATURE_TROLL;
                break;
            case REINFORCEMENT_TIER_ONE_KEY + IDX(FACTION_SORCERESS):
                creature28 = CREATURE_SPRITE;
                break;
            case REINFORCEMENT_TIER_TWO_KEY + IDX(FACTION_SORCERESS):
                creature28 = CREATURE_DWARF;
                break;
            case REINFORCEMENT_TIER_THREE_KEY + IDX(FACTION_SORCERESS):
                creature28 = CREATURE_ELF;
                break;
            case REINFORCEMENT_TIER_FOUR_KEY + IDX(FACTION_SORCERESS):
                creature28 = CREATURE_DRUID;
                break;
            case REINFORCEMENT_TIER_FIVE_KEY + IDX(FACTION_SORCERESS):
                creature28 = CREATURE_UNICORN;
                break;
            case REINFORCEMENT_TIER_ONE_KEY + IDX(FACTION_WARLOCK):
                creature28 = CREATURE_CENTAUR;
                break;
            case REINFORCEMENT_TIER_TWO_KEY + IDX(FACTION_WARLOCK):
                creature28 = CREATURE_GARGOYLE;
                break;
            case REINFORCEMENT_TIER_THREE_KEY + IDX(FACTION_WARLOCK):
                creature28 = CREATURE_GRIFFIN;
                break;
            case REINFORCEMENT_TIER_FOUR_KEY + IDX(FACTION_WARLOCK):
                creature28 = CREATURE_MINOTAUR;
                break;
            case REINFORCEMENT_TIER_FIVE_KEY + IDX(FACTION_WARLOCK):
                creature28 = CREATURE_HYDRA;
                break;
            case REINFORCEMENT_TIER_ONE_KEY + IDX(FACTION_WIZARD):
                creature28 = CREATURE_HALFLING;
                break;
            case REINFORCEMENT_TIER_TWO_KEY + IDX(FACTION_WIZARD):
                creature28 = CREATURE_BOAR;
                break;
            case REINFORCEMENT_TIER_THREE_KEY + IDX(FACTION_WIZARD):
                creature28 = CREATURE_IRON_GOLEM;
                break;
            case REINFORCEMENT_TIER_FOUR_KEY + IDX(FACTION_WIZARD):
                creature28 = CREATURE_ROC;
                break;
            case REINFORCEMENT_TIER_FIVE_KEY + IDX(FACTION_WIZARD):
                creature28 = CREATURE_MAGE;
                break;
            case REINFORCEMENT_TIER_ONE_KEY + IDX(FACTION_NECROMANCER):
                creature28 = CREATURE_SKELETON;
                break;
            case REINFORCEMENT_TIER_TWO_KEY + IDX(FACTION_NECROMANCER):
                creature28 = CREATURE_ZOMBIE;
                break;
            case REINFORCEMENT_TIER_THREE_KEY + IDX(FACTION_NECROMANCER):
                creature28 = CREATURE_MUMMY;
                break;
            case REINFORCEMENT_TIER_FOUR_KEY + IDX(FACTION_NECROMANCER):
                creature28 = CREATURE_VAMPIRE;
                break;
            case REINFORCEMENT_TIER_FIVE_KEY + IDX(FACTION_NECROMANCER):
                creature28 = CREATURE_LICH;
                break;
            default:;
        }
        GiveArmy(&m_castleRecs[townId].m_army, creature28, count1, ARMY_GROUP_EMPTY_SLOT);
    }
}

VA(0x004745d4, 0xa4)
void game::GiveTroopsToNeutralTowns(void) {
    i32 i;
    for (i = 0; i < GAME_TOWN_COUNT; i++) {
        GiveTroopsToNeutralTown(i);
        if (m_castleRecs[i].m_buildings & IDX(TOWN_BUILDING_CASTLE)) {
            if (Random(0, REINFORCEMENT_ROLL_PERCENT_MAX) < REINFORCEMENT_CASTLE_CHANCE)
                GiveTroopsToNeutralTown(i);
        } else {
            if (Random(0, REINFORCEMENT_ROLL_PERCENT_MAX) < REINFORCEMENT_CHANCE)
                GiveTroopsToNeutralTown(i);
        }
    }
}

VA(0x00474678, 0x1dd0)
void game::NewMap(char* filename) {
    char* extension0;
    FactionType randomColor2;
    i32 nextHuman6;
    i32 player2;
    i32 townIndex9;
    i32 heroIndex1;
    i32 pass27;
    i32 selectedTown14;
    i32 ultimateDistance5;
    i32 ultimateTries4;
    i32 campaignHero15;
    FactionType heroClass5;
    i32 heroX6;
    i32 heroY16;
    i8 setupClass12;
    FactionType specialFaction6;
    HeroPortrait specialPortrait6;
    char* specialName3;
    FactionType specialClass6;
    i32 resource13;

    extension0 = FindLastToken(gMapName, '.');
    if (extension0 != NULL && StrEqNoCase(extension0 + 1, DATA_COMPGEN(0x004f7250, newMapMX2, "MX2")))
        xIsExpansionMap = 1;
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
    randomColor2 = static_cast<FactionType>(Random(0, GAME_PLAYER_COUNT - 1));
    nextHuman6 = giNumHumanPlayers;

    for (player2 = 0; player2 < GAME_PLAYER_COUNT; player2++) {
        if (player2 >= gpGame->m_mapHeader.playerCount) {
            gbSetupGamePosToRealGamePos[player2] = -1;
        } else {
            if (m_setupPlayerNetworkId[player2] == GAME_COMPUTER_PLAYER)
                gbSetupGamePosToRealGamePos[player2] = static_cast<i8>(nextHuman6++);
            else
                gbSetupGamePosToRealGamePos[player2] = m_setupPlayerNetworkId[player2];
        }
    }
    for (player2 = 0; player2 < GAME_PLAYER_COUNT; player2++) {
        m_players[player2].m_color = -1;
        gcColorToPlayerPos[player2] = -1;
        gcColorToSetupPos[player2] = -1;
        if (gpGame->m_setupPlayerRace[player2] == FACTION_RANDOM)
            gpGame->m_setupPlayerRace[player2] = randomColor2;
        randomColor2 = (randomColor2 + 1) % GAME_PLAYER_COUNT;
    }
    for (player2 = 0; player2 < m_playerCount; player2++)
        gcColorToSetupPos[m_setupPlayerColor[player2]] = static_cast<i8>(player2);
    for (player2 = 0; player2 < m_playerCount; player2++)
        m_players[gbSetupGamePosToRealGamePos[player2]].m_color = m_setupPlayerColor[player2];
    for (player2 = 0; player2 < m_playerCount; player2++)
        gcColorToPlayerPos[m_players[player2].m_color] = static_cast<i8>(player2);
    for (player2 = 0; player2 < m_playerCount; player2++) {
        m_players[player2].m_townCount = 0;
        m_players[player2].m_townLocatorPage = 0;
        m_players[player2].m_currentTown = -1;
        m_players[player2].m_heroCount = 0;
        m_players[player2].m_heroLocatorPage = 0;
        m_players[player2].m_currentHero = -1;
    }

    RandomizeHeroPool();
    strcpy(gMapName, filename);
    LoadMap(gMapName);
    InitRandomArtifacts();
    ProcessRandomObjects();
    ProcessMapExtra();
    SetupTowns();
    InitializePasswords();
    for (player2 = 0; player2 < GAME_PLAYER_COUNT; player2++)
        m_players[player2].m_barrierTents = 0;
    RandomizeEvents();
    ProcessOnMapHeroes();
    m_deadPlayerCount = 0;
    for (player2 = m_playerCount; player2 < GAME_PLAYER_COUNT; player2++)
        m_playerDead[player2] = 1;

    if (m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_SIDE
        || m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO) {
        m_mapHeader.computerAlsoWins = 1;
        m_mapHeader.allowNormalVictory = 0;
    }
    if (m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_SIDE) {
        townIndex9 = 0;
        for (player2 = 0; player2 < GAME_PLAYER_COUNT; player2++) {
            if (m_mapHeader.playerEnabled[player2] != 0)
                townIndex9++;
            if (m_mapHeader.victoryConditionValue + 1 == townIndex9) {
                m_mapHeader.victorySideThreshold = static_cast<u16>(player2);
                player2 = VICTORY_SIDE_SEARCH_DONE;
            }
        }
    }
    if (m_mapHeader.victoryCondition == MAP_VICTORY_FIND_ARTIFACT)
        m_mapHeader.computerAlsoWins = 1;

    for (player2 = 0; player2 < m_playerCount; player2++) {
        m_players[player2].m_ultimateArtifactHintChance = 0;
        m_players[player2].m_ultimateArtifactHintX = -1;
        m_players[player2].m_ultimateArtifactHintY = -1;
        heroIndex1 = 0;
        selectedTown14 = -1;
        if (m_mapHeader.unknown25 == 0 && m_players[player2].m_townCount > 0) {
            for (pass27 = 0; pass27 < STARTING_HERO_TOWN_PASS_COUNT; pass27++) {
                for (townIndex9 = 0; townIndex9 < m_players[player2].m_townCount; townIndex9++) {
                    if (selectedTown14 == -1
                        && m_castleRecs[(m_players + player2)->m_townIds[townIndex9]]
                                   .m_occupyingHeroId
                               == -1
                        && ((m_castleRecs[(m_players + player2)->m_townIds[townIndex9]].m_buildings
                             & IDX(TOWN_BUILDING_CASTLE))
                                != 0
                            || pass27 == STARTING_HERO_ALLOW_NON_CASTLE_PASS))
                        selectedTown14 = townIndex9;
                }
            }
        }
        if (selectedTown14 != -1) {
            m_players[player2].m_heroIds[m_players[player2].m_heroCount] =
                static_cast<i8>(GetNewHeroId(
                    player2,
                    m_castleRecs[m_players[player2].m_townIds[selectedTown14]].m_type,
                    0
                ));
            m_availableHeroes[m_players[player2].m_heroIds[m_players[player2].m_heroCount]] =
                static_cast<i8>(player2);
            m_heroRecs[m_players[player2].m_heroIds[m_players[player2].m_heroCount]].m_owner =
                static_cast<i8>(player2);
            m_heroRecs[m_players[player2].m_heroIds[m_players[player2].m_heroCount]].m_x =
                m_castleRecs[m_players[player2].m_townIds[selectedTown14]].m_x;
            m_heroRecs[m_players[player2].m_heroIds[m_players[player2].m_heroCount]].m_y =
                m_castleRecs[m_players[player2].m_townIds[selectedTown14]].m_y;
            m_castleRecs[m_players[player2].m_townIds[selectedTown14]].m_occupyingHeroId =
                m_players[player2].m_heroIds[m_players[player2].m_heroCount];
            SetVisibility(
                m_heroRecs[m_players[player2].m_heroIds[m_players[player2].m_heroCount]].m_x,
                m_heroRecs[m_players[player2].m_heroIds[m_players[player2].m_heroCount]].m_y,
                player2,
                giVisRange[static_cast<i8>(
                    m_heroRecs[m_players[player2].m_heroIds[0]].m_cursorType)]
            );
            m_players[player2].m_heroCount++;
        }
    }

    for (player2 = 0; player2 < m_playerCount; player2++) {
        if (player2 == 0 && gbInCampaign
            && (m_campaignAwards[IDX(CAMPAIGN_AWARD_SORCERESS_GUILD)] != 0
                || m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARFBANE)] != 0)) {
            if (m_campaignAwards[IDX(CAMPAIGN_AWARD_SORCERESS_GUILD)] != 0)
                specialFaction6 = FACTION_SORCERESS;
            else
                specialFaction6 = FACTION_NECROMANCER;
            for (campaignHero15 = 0; campaignHero15 < GAME_HERO_COUNT; campaignHero15++) {
                if (m_heroRecs[campaignHero15].m_cursorType == specialFaction6
                    && m_availableHeroes[campaignHero15] == -1)
                    break;
            }
            if (campaignHero15 < GAME_HERO_COUNT) {
                if (m_campaignAwards[IDX(CAMPAIGN_AWARD_SORCERESS_GUILD)] != 0) {
                    m_heroRecs[campaignHero15].m_experience += CAMPAIGN_EXPERIENCE_BONUS;
                    m_heroRecs[campaignHero15].CheckLevel();
                    strcpy(m_heroRecs[campaignHero15].m_name, DATA_COMPGEN(0x004f7254, newMapSisterEliza, "Sister Eliza"));
                    m_heroRecs[campaignHero15].m_portrait = CAMPAIGN_HERO_ELIZA;
                } else {
                    m_heroRecs[campaignHero15].m_experience += CAMPAIGN_EXPERIENCE_BONUS;
                    m_heroRecs[campaignHero15].CheckLevel();
                    strcpy(m_heroRecs[campaignHero15].m_name, DATA_COMPGEN(0x004f7264, newMapBrotherBrax, "Brother Brax"));
                    m_heroRecs[campaignHero15].m_portrait = CAMPAIGN_HERO_BRAX;
                }
                m_players[player2].m_availableHeroIds[0] = static_cast<char>(campaignHero15);
                m_availableHeroes[m_players[player2].m_availableHeroIds[0]] =
                    WEEKLY_AVAILABLE_HERO;
                heroClass5 = m_heroRecs[campaignHero15].m_cursorType;
                goto secondHero;
            }
        }
        {
            if (xIsPlayingExpansionCampaign && player2 == 0) {
                specialClass6 = FACTION_ANY;
                if (xCampaign.HasAward(AWARD_WAYWARD_SON)) {
                    specialClass6 = FACTION_WIZARD;
                    specialName3 = xCampaign.JosephName();
                    specialPortrait6 = EXPANSION_HERO_JOSEPH_PORTRAIT;
                } else if (xCampaign.HasAward(AWARD_UNCLE_IVAN)) {
                    specialClass6 = FACTION_BARBARIAN;
                    specialName3 = xCampaign.IvanName();
                    specialPortrait6 = EXPANSION_HERO_IVAN_PORTRAIT;
                }
                if (specialClass6 != FACTION_ANY) {
                    for (campaignHero15 = 0; campaignHero15 < GAME_HERO_COUNT; campaignHero15++) {
                        if (m_heroRecs[campaignHero15].m_cursorType == specialClass6
                            && m_availableHeroes[campaignHero15] == -1)
                            break;
                    }
                    if (campaignHero15 < GAME_HERO_COUNT) {
                        m_heroRecs[campaignHero15].m_experience = CAMPAIGN_EXPERIENCE_BONUS;
                        m_heroRecs[campaignHero15].CheckLevel();
                        strcpy(m_heroRecs[campaignHero15].m_name, specialName3);
                        m_heroRecs[campaignHero15].m_portrait = specialPortrait6;
                        m_players[player2].m_availableHeroIds[0] =
                            static_cast<char>(campaignHero15);
                        m_availableHeroes[m_players[player2].m_availableHeroIds[0]] =
                            WEEKLY_AVAILABLE_HERO;
                        heroClass5 = m_heroRecs[campaignHero15].m_cursorType;
                        goto secondHero;
                    }
                }
            }
            heroClass5 = static_cast<FactionType>(Random(0, IDX(FACTION_COUNT) - 1));
            if (m_setupPlayerRace[gcColorToSetupPos[m_players[player2].m_color]]
                < FACTION_COUNT)
                heroClass5 = m_setupPlayerRace[gcColorToSetupPos[m_players[player2].m_color]];
            m_players[player2].m_availableHeroIds[0] =
                static_cast<char>(GetNewHeroId(player2, heroClass5, 0));
            m_availableHeroes[m_players[player2].m_availableHeroIds[0]] = WEEKLY_AVAILABLE_HERO;
        }
    secondHero:
        heroClass5 = (heroClass5 + Random(1, IDX(FACTION_COUNT) - 1)) % IDX(FACTION_COUNT);
        m_players[player2].m_availableHeroIds[1] =
            static_cast<char>(GetNewHeroId(player2, heroClass5, 0));
        m_availableHeroes[m_players[player2].m_availableHeroIds[1]] = WEEKLY_AVAILABLE_HERO;
    }

    for (player2 = 0; player2 < m_playerCount; player2++) {
        for (campaignHero15 = 0; campaignHero15 < m_players[player2].m_heroCount;
             campaignHero15++) {
            heroX6 = m_heroRecs[m_players[player2].m_heroIds[campaignHero15]].m_x;
            heroY16 = m_heroRecs[m_players[player2].m_heroIds[campaignHero15]].m_y;
            m_heroRecs[m_players[player2].m_heroIds[campaignHero15]].m_locationType =
                m_worldMap.GetCell(heroX6, heroY16)->m_triggerType;
            m_heroRecs[m_players[player2].m_heroIds[campaignHero15]].m_occupiedTown =
                m_worldMap.GetCell(heroX6, heroY16)->m_objectMetadata;
            m_worldMap.GetCell(heroX6, heroY16)->m_triggerType =
                MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION;
            m_worldMap.GetCell(heroX6, heroY16)->m_objectMetadata =
                m_players[player2].m_heroIds[campaignHero15];
        }
        if (m_players[player2].m_heroCount > 0)
            m_players[player2].m_currentHero = m_players[player2].m_heroIds[0];
        else if (m_players[player2].m_townCount > 0)
            m_players[player2].m_currentTown = m_players[player2].m_townIds[0];
    }

    player2 = -1;
    townIndex9 = -1;
    ultimateTries4 = 0;
    ultimateDistance5 =
        Random(ULTIMATE_DISTANCE_ROLL_MIN, ULTIMATE_DISTANCE_COMMON_ROLL_MAX)
        + Random(ULTIMATE_DISTANCE_ROLL_MIN, ULTIMATE_DISTANCE_COMMON_ROLL_MAX)
        + Random(ULTIMATE_DISTANCE_ROLL_MIN, ULTIMATE_DISTANCE_BONUS_ROLL_MAX);
    while (player2 < ULTIMATE_ARTIFACT_BORDER_MARGIN
           || townIndex9 < ULTIMATE_ARTIFACT_BORDER_MARGIN
           || player2 > MAP_WIDTH - ULTIMATE_ARTIFACT_BORDER_MARGIN - 1
           || townIndex9 > MAP_HEIGHT - ULTIMATE_ARTIFACT_BORDER_MARGIN - 1
           || m_worldMap.GetCell(player2, townIndex9)->m_objectIndex != MAPCELL_SPRITE_NONE
           || m_worldMap.GetCell(player2, townIndex9)->m_overlayIndex != MAPCELL_SPRITE_NONE
           || giGroundToTerrain[m_worldMap.GetCell(player2, townIndex9)->m_terrainImageIndex]
                  == TERRAIN_WATER
           || (giNumHumanPlayers == 1
               && ultimateTries4 < ULTIMATE_HUMAN_DISTANCE_RETRY_LIMIT
               && ultimateDistance5
                      >= abs(player2 - m_heroRecs[m_players[0].m_heroIds[0]].m_x)
                             + abs(townIndex9 - m_heroRecs[m_players[0].m_heroIds[0]].m_y))) {
        if (ultimateTries4 < ULTIMATE_SEARCH_REGION_RETRY_LIMIT && giUABaseX > 0) {
            player2 = giUABaseX + (giUARadius != 0 ? Random(-giUARadius, giUARadius) : 0);
            townIndex9 = giUABaseY + (giUARadius != 0 ? Random(-giUARadius, giUARadius) : 0);
        } else {
            player2 = Random(
                ULTIMATE_ARTIFACT_BORDER_MARGIN,
                MAP_WIDTH - ULTIMATE_ARTIFACT_BORDER_MARGIN - 1
            );
            townIndex9 = Random(
                ULTIMATE_ARTIFACT_BORDER_MARGIN,
                MAP_HEIGHT - ULTIMATE_ARTIFACT_BORDER_MARGIN - 1
            );
        }
        ultimateDistance5 =
            Random(ULTIMATE_DISTANCE_ROLL_MIN, ULTIMATE_DISTANCE_COMMON_ROLL_MAX)
            + Random(ULTIMATE_DISTANCE_ROLL_MIN, ULTIMATE_DISTANCE_COMMON_ROLL_MAX)
            + Random(ULTIMATE_DISTANCE_ROLL_MIN, ULTIMATE_DISTANCE_BONUS_ROLL_MAX);
        ultimateTries4++;
    }
    m_ultimateArtifactX = static_cast<i8>(player2);
    m_ultimateArtifactY = static_cast<i8>(townIndex9);
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
    for (player2 = 0; player2 < m_playerCount; player2++) {
        if (gbHumanPlayer[player2]) {
            m_players[player2].m_aiDifficulty = PLAYER_PERSONALITY_HUMAN;
            memcpy(
                m_players[player2].m_resources,
                gInitResourcesHuman[IDX(m_difficulty)],
                sizeof(m_players[player2].m_resources)
            );
            if (m_playerHandicap[player2] != PLAYER_HANDICAP_NONE) {
                for (townIndex9 = 0; townIndex9 < IDX(RES_COUNT); townIndex9++) {
                    double resourceScale;
                    if (m_playerHandicap[player2] == PLAYER_HANDICAP_MODERATE)
                        resourceScale = GAME_HANDICAP_MODERATE_RESOURCE_FACTOR;
                    else
                        resourceScale = GAME_HANDICAP_SEVERE_RESOURCE_FACTOR;
                    (m_players + player2)->m_resources[townIndex9] = static_cast<i32>(
                        (m_players + player2)->m_resources[townIndex9] * resourceScale
                    );
                }
            }
        } else {
            m_players[player2].m_aiDifficulty = static_cast<PlayerPersonality>(Random(
                IDX(PLAYER_PERSONALITY_COMPUTER_FIRST),
                IDX(PLAYER_PERSONALITY_COMPUTER_LAST)
            ));
            memcpy(
                m_players[player2].m_resources,
                gInitResourcesComputer[IDX(m_difficulty)],
                sizeof(m_players[player2].m_resources)
            );
        }
    }
    SetupAdjacentMons();
    if (m_mapHeader.lossCondition == MAP_LOSS_HERO) {
        ultimateDistance5 = m_mapHeader.lossConditionValue;
        ultimateTries4 = m_mapHeader.lossTownY;
        m_mapHeader.lossConditionValue = 0;
        if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->m_triggerType
            == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID))
            m_mapHeader.lossConditionValue =
                m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->m_objectMetadata;
        else {
            if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->m_triggerType
                == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID))
                m_mapHeader.lossConditionValue =
                    m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->m_objectMetadata;
            else
                m_mapHeader.lossCondition = MAP_LOSS_STANDARD;
        }
    }
    if (m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO) {
        ultimateDistance5 = m_mapHeader.victoryConditionValue;
        ultimateTries4 = m_mapHeader.victoryTownY;
        m_mapHeader.victoryConditionValue = 0;
        if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->m_triggerType
            == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID))
            m_mapHeader.victoryConditionValue =
                m_worldMap.GetCell(ultimateDistance5, ultimateTries4)->m_objectMetadata;
        else {
            if (m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->m_triggerType
                == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID))
                m_mapHeader.victoryConditionValue =
                    m_worldMap.GetCell(ultimateDistance5, ultimateTries4 - 1)->m_objectMetadata;
            else
                m_mapHeader.victoryCondition = MAP_VICTORY_DEFEAT_ALL;
        }
    }
    for (player2 = 0; player2 < m_playerCount; player2++) {
        heroClass5 = FACTION_KNIGHT;
        if (m_setupPlayerRace[gcColorToSetupPos[m_players[player2].m_color]] >= FACTION_KNIGHT
            && m_setupPlayerRace[gcColorToSetupPos[m_players[player2].m_color]]
                   < FACTION_COUNT) {
            heroClass5 = m_setupPlayerRace[gcColorToSetupPos[m_players[player2].m_color]];
        } else {
            if (!!m_players[player2].m_townCount) {
                heroClass5 = gpGame->m_castleRecs[m_players[player2].TownId(0)].m_type;
            } else if (!!m_players[player2].m_heroCount) {
                heroClass5 = gpGame->m_heroRecs[m_players[player2].HeroId(0)].m_cursorType;
            }
        }
        m_players[player2].m_evilInterface = heroClass5 == FACTION_BARBARIAN
                                             || heroClass5 == FACTION_WARLOCK
                                             || heroClass5 == FACTION_NECROMANCER;
        if (gbInCampaign && player2 == 0)
            m_players[player2].m_evilInterface = m_campaignType == CAMPAIGN_ARCHIBALD;
        for (townIndex9 = 0; townIndex9 < gpGame->m_players[player2].m_townCount; townIndex9++)
            GetCastle(gpGame->m_players[player2].m_townIds[townIndex9])->GiveSpells(NULL);
        gpGame->m_players[player2].m_minimumHeroCount = gpGame->m_players[player2].m_heroCount;
    }
    gpPhilAI->GetGameAIVars();
    gbInNewGameSetup = false;
    SetupNewRumour();
    gpAdvManager->CheckSetEvilInterface(0, -1);
    return;
}

inline town* GetCastleSlot(game* instance, i32 index) {
    return &instance->m_castleRecs[index];
}

VA(0x00476448, 0x2601)
void game::RandomizeEvents(void) {
    i32 shrineId8 = 1;
    i32 bottleId11 = 1;
    i32 jailId28 = 1;
    i32 sphinxId26 = 1;
    i32 tentId10 = 1;
    i32 hutId11 = 1;
    i32 eyeId13 = 1;
    i32 row18;
    i32 signId4 = 1;
    u32 extraIndex3;
    i32 yPos19;
    i32 xPos2;
    i32 j9;
    ArtifactType value26;
    i32 randomValue7;
    i32 mineId2;
    i32 column1;
    i32 upperCount;
    i32 lowerCount16;
    TilesetId upperTilesets29[LAYER_SCAN_CAPACITY];
    i32 upperIndexes1[LAYER_SCAN_CAPACITY];
    TilesetId lowerTilesets4[LAYER_SCAN_CAPACITY];
    i32 lowerIndexes7[LAYER_SCAN_CAPACITY];
    CreatureType artifactGuardianChoices[ARTIFACT_GUARDIAN_CHOICE_COUNT];
    EventExtra* mapEvent1;
    mapCell* townEntrance;
    mapCell* cell2;
    mapCellExtra* extra15;
    town* townRec4;
    mapEventExtra* eventData16;
    i32 valid27;

    m_mapEventCount = 0;
    memset(m_mapEventIndices, 0, sizeof(m_mapEventIndices));

    for (yPos19 = 0; yPos19 < MAP_HEIGHT; yPos19++) {
        for (xPos2 = 0; xPos2 < MAP_WIDTH; xPos2++) {
            cell2 = m_worldMap.Row(yPos19) + xPos2;
            switch (cell2->m_triggerType) {
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WITCH_HUT:
                    cell2->m_objectMetadata = IDX(HERO_SKILL_NECROMANCY);
                    while (cell2->m_objectMetadata == IDX(HERO_SKILL_NECROMANCY)
                           || cell2->m_objectMetadata == IDX(HERO_SKILL_LEADERSHIP)) {
                        cell2->m_objectMetadata =
                            Random(WITCH_HUT_SKILL_FIRST, WITCH_HUT_SKILL_LAST);
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_BOAT:
                    cell2->m_objectTileset = TILESET_NONE;
                    cell2->m_objectIndex = MAPCELL_SPRITE_NONE;
                    cell2->m_objectMetadata = 0;
                    cell2->m_triggerType = 0;
                    CreateBoat(xPos2, yPos19, 1);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SPHINX:
                    eventData16 =
                        reinterpret_cast<mapEventExtra*>(ppMapExtra[cell2->m_objectMetadata]);
                    if (strlen(eventData16->riddle) > 1 && eventData16->answerCount >= 1)
                        eventData16->active = 1;
                    else
                        eventData16->active = 0;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MAP_EVENT:
                    m_mapEventIndices[m_mapEventCount] = cell2->m_objectMetadata;
                    mapEvent1 = reinterpret_cast<EventExtra*>(ppMapExtra[cell2->m_objectMetadata]);
                    mapEvent1->x = static_cast<i16>(xPos2);
                    mapEvent1->y = static_cast<i16>(yPos19);
                    mapEvent1->active = 1;
                    cell2->m_objectMetadata = 0;
                    cell2->m_triggerType = 0;
                    cell2->m_objectIndex = MAPCELL_SPRITE_NONE;
                    cell2->m_objectTileset = TILESET_NONE;
                    m_mapEventCount++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_GAZEBO:
                    cell2->m_objectMetadata = bottleId11++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_FORT:
                    cell2->m_objectMetadata = jailId28++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WITCH_DOCTOR_HUT:
                    cell2->m_objectMetadata = sphinxId26;
                    sphinxId26++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERCENARY_CAMP:
                    cell2->m_objectMetadata = tentId10++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_STANDING_STONES:
                    if (xPos2 <= 0
                        || m_worldMap.GetCell(xPos2 - 1, yPos19)->m_triggerType
                               != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_STANDING_STONES))
                        cell2->m_objectMetadata = hutId11++;
                    else
                        cell2->m_objectMetadata =
                            m_worldMap.GetCell(xPos2 - 1, yPos19)->m_objectMetadata;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_XANADU:
                    cell2->m_objectMetadata = signId4++;
                    break;
                case MAP_OBJECT_WHIRLPOOL:
                    cell2->m_triggerType |= MAP_TRIGGER_ACTION_FLAG;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_OBELISK:
                    cell2->m_objectMetadata = shrineId8++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_FLOTSAM:
                    cell2->m_objectMetadata =
                        Random(IDX(FLOTSAM_EMPTY), IDX(FLOTSAM_LARGE_TREASURE));
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SKELETON:
                    if (!HasObjectTilesetIndex(
                            xPos2,
                            yPos19,
                            TILESET_OBJNDSRT,
                            SKELETON_DESERT_FRAME
                        )) {
                        cell2->m_triggerType &= MAP_TRIGGER_TYPE_MASK;
                    } else if (Random(SKELETON_ROLL_MIN, SKELETON_ROLL_MAX)
                               > SKELETON_ARTIFACT_ROLL_MAX) {
                        cell2->m_objectMetadata = SKELETON_EMPTY;
                    } else {
                        cell2->m_objectMetadata =
                            GetRandomArtifactId(ARTIFACT_LEVEL_RANDOM, true)
                            + SKELETON_ARTIFACT_OFFSET;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WAGON:
                    randomValue7 = Random(EVENT_ROLL_MIN, EVENT_ROLL_MAX);
                    if (randomValue7 < WAGON_EMPTY_CUTOFF)
                        cell2->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                    else if (randomValue7 < WAGON_ARTIFACT_CUTOFF)
                        cell2->m_objectMetadata =
                            GetRandomArtifactId(
                                ARTIFACT_LEVEL_MINOR | ARTIFACT_LEVEL_TREASURE,
                                true
                            )
                            | WAGON_ARTIFACT_FLAG;
                    else
                        cell2->m_objectMetadata =
                            Random(IDX(RES_WOOD), IDX(RES_GEMS))
                            + (Random(WAGON_AMOUNT_MIN, WAGON_AMOUNT_MAX)
                               << CAMPFIRE_AMOUNT_SHIFT)
                            + MAP_EVENT_RESOURCE_OFFSET;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_LEAN_TO:
                    cell2->m_objectMetadata =
                        Random(IDX(RES_WOOD), IDX(RES_GEMS))
                        + (Random(LEAN_TO_AMOUNT_MIN, LEAN_TO_AMOUNT_MAX)
                           << CAMPFIRE_AMOUNT_SHIFT)
                        + MAP_EVENT_RESOURCE_OFFSET;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DAEMON_CAVE:
                    // Decimal buckets are balance payload; the stored values are event outcomes.
                    // NOLINTBEGIN(readability-magic-numbers)
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
                    // NOLINTEND(readability-magic-numbers)
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREASURE_CHEST:
                    if (giGroundToTerrain[cell2->m_terrainImageIndex] == TERRAIN_WATER) {
                        cell2->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SEA_CHEST;
                        randomValue7 = Random(EVENT_ROLL_MIN, EVENT_ROLL_MAX);
                        if (randomValue7 < SEA_CHEST_EMPTY_CUTOFF)
                            cell2->m_objectMetadata = SEA_CHEST_OUTCOME_EMPTY;
                        else if (randomValue7 < SEA_CHEST_GOLD_CUTOFF)
                            cell2->m_objectMetadata = SEA_CHEST_OUTCOME_GOLD;
                        else
                            cell2->m_objectMetadata =
                                GetRandomArtifactId(ARTIFACT_LEVEL_TREASURE, true)
                                | CHEST_ARTIFACT_FLAG;
                    } else {
                        randomValue7 = Random(EVENT_ROLL_MIN, EVENT_ROLL_MAX);
                        if (randomValue7 < LAND_CHEST_SMALL_CUTOFF)
                            cell2->m_objectMetadata = CHEST_REWARD_SMALL;
                        else if (randomValue7 < LAND_CHEST_MEDIUM_CUTOFF)
                            cell2->m_objectMetadata = CHEST_REWARD_MEDIUM;
                        else if (randomValue7 < LAND_CHEST_LARGE_CUTOFF)
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
                    cell2->m_objectMetadata |= Random(IDX(RES_WOOD), IDX(RES_GEMS));
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ANCIENT_LAMP:
                    cell2->m_objectMetadata =
                        Random(ANCIENT_LAMP_ROLL_MIN, ANCIENT_LAMP_ROLL_MAX)
                        + ANCIENT_LAMP_COUNT_OFFSET;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHIPWRECK_SURVIVOR:
                    randomValue7 = Random(EVENT_ROLL_MIN, EVENT_ROLL_MAX);
                    if (randomValue7 < SHIPWRECK_SURVIVOR_TREASURE_CUTOFF)
                        cell2->m_objectMetadata =
                            GetRandomArtifactId(ARTIFACT_LEVEL_TREASURE, true);
                    else if (randomValue7 < SHIPWRECK_SURVIVOR_MINOR_CUTOFF)
                        cell2->m_objectMetadata = GetRandomArtifactId(ARTIFACT_LEVEL_MINOR, true);
                    else
                        cell2->m_objectMetadata = GetRandomArtifactId(ARTIFACT_LEVEL_MAJOR, true);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_GRAVEYARD:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHIPWRECK:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DERELICT_SHIP:
                    // Decimal buckets are balance payload; the stored value selects encounter size.
                    // NOLINTBEGIN(readability-magic-numbers)
                    switch (Random(EVENT_ROLL_MIN, EVENT_BUCKET_ROLL_MAX) % EVENT_BUCKET_COUNT) {
                        case 0:
                        case 1:
                        case 2:
                            cell2->m_objectMetadata = IDX(EVENT_LEVEL_SMALL);
                            break;
                        case 3:
                        case 4:
                        case 5:
                            cell2->m_objectMetadata = IDX(EVENT_LEVEL_MEDIUM);
                            break;
                        case 6:
                        case 7:
                        case 8:
                            cell2->m_objectMetadata = IDX(EVENT_LEVEL_LARGE);
                            break;
                        case 9:
                            cell2->m_objectMetadata = IDX(EVENT_LEVEL_HUGE);
                            break;
                    }
                    // NOLINTEND(readability-magic-numbers)
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARCHER_HOUSE:
                    // Retail weekly dwelling availability ranges are balance payload.
                    // NOLINTBEGIN(readability-magic-numbers)
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
                    // NOLINTEND(readability-magic-numbers)
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
                            ? IDX(RES_GEMS) + MAP_EVENT_RESOURCE_OFFSET
                            : IDX(RES_GOLD) + MAP_EVENT_RESOURCE_OFFSET;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_OF_KNOWLEDGE:
                    cell2->m_objectMetadata =
                        (Random(TREE_KNOWLEDGE_FREE, TREE_KNOWLEDGE_GEMS)
                         << TREE_KNOWLEDGE_MODE_SHIFT)
                        | eyeId13++;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER: {
                    CreatureType monsterType = static_cast<CreatureType>(cell2->m_objectIndex);
                    if (cell2->m_objectMetadata == MAP_EVENT_DATA_EMPTY) {
                        cell2->m_objectMetadata = GetRandomNumTroops(monsterType);
                        if (CanGenerateMonsterGuard(monsterType)
                            && Random(MONSTER_GUARD_ROLL_MIN, MONSTER_GUARD_ROLL_MAX)
                                   < MONSTER_GUARD_CUTOFF)
                            cell2->m_objectMetadata |= IDX(MAP_MONSTER_GUARD_FLAG);
                    }
                    break;
                }
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RESOURCE: {
                    ResourceType resourceType =
                        static_cast<ResourceType>(cell2->m_objectIndex >> 1);
                    cell2->m_objectMetadata = IDX(resourceType);
                    switch (resourceType) {
                        case RES_WOOD:
                        case RES_ORE:
                            cell2->m_objectMetadata =
                                Random(RESOURCE_BULK_AMOUNT_MIN, RESOURCE_BULK_AMOUNT_MAX);
                            break;
                        case RES_GOLD:
                            cell2->m_objectMetadata =
                                Random(RESOURCE_BULK_AMOUNT_MIN, RESOURCE_BULK_AMOUNT_MAX);
                            break;
                        default:
                            cell2->m_objectMetadata =
                                Random(RESOURCE_SCARCE_AMOUNT_MIN, RESOURCE_SCARCE_AMOUNT_MAX);
                            break;
                    }
                    break;
                }
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHRINE_FIRST_CIRCLE:
                    cell2->m_objectMetadata =
                        GetRandomEventSpell(SPELL_LEVEL_FIRST);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHRINE_SECOND_CIRCLE:
                    cell2->m_objectMetadata =
                        GetRandomEventSpell(SPELL_LEVEL_SECOND);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SHRINE_THIRD_CIRCLE:
                    cell2->m_objectMetadata =
                        GetRandomEventSpell(SPELL_LEVEL_THIRD);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_PYRAMID:
                    cell2->m_objectMetadata = GetRandomEventSpell(SPELL_LEVEL_FIFTH);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_HOUSE:
                    // Retail recruit-site and guarded-bank quantities are balance payload.
                    // NOLINTBEGIN(readability-magic-numbers)
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
                            xPos2,
                            yPos19,
                            TILESET_OBJNMUL2,
                            WAGON_CAMP_ACTIVE_FRAME
                        ))
                        cell2->m_triggerType &= MAP_TRIGGER_TYPE_MASK;
                    else
                        cell2->m_objectMetadata = Random(30, 50);
                    // NOLINTEND(readability-magic-numbers)
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT:
                    randomValue7 = Random(EVENT_ROLL_MIN, EVENT_BUCKET_ROLL_MAX);
                    value26 = static_cast<ArtifactType>(cell2->m_objectIndex >> 1);
                    if (value26 != ARTIFACT_SPELL_SCROLL) {
                        if (randomValue7 < ARTIFACT_EVENT_UNCONDITIONAL_CUTOFF) {
                            if (randomValue7 % EVENT_BUCKET_COUNT
                                == ARTIFACT_EVENT_WISDOM_BUCKET)
                                cell2->m_objectMetadata = ARTIFACT_EVENT_MODE_WISDOM;
                            else if (randomValue7 % EVENT_BUCKET_COUNT
                                     == ARTIFACT_EVENT_LEADERSHIP_BUCKET)
                                cell2->m_objectMetadata = ARTIFACT_EVENT_MODE_LEADERSHIP;
                            else
                                cell2->m_objectMetadata = ARTIFACT_EVENT_MODE_PICKUP;
                        } else if (randomValue7 < ARTIFACT_EVENT_GUARD_CUTOFF) {
                            if (gArtifactLevel[IDX(value26)] == ARTIFACT_LEVEL_TREASURE)
                                cell2->m_objectMetadata = ARTIFACT_EVENT_MODE_GOLD;
                            else if (gArtifactLevel[IDX(value26)] == ARTIFACT_LEVEL_MINOR)
                                cell2->m_objectMetadata =
                                    (Random(IDX(RES_WOOD), IDX(RES_GEMS))
                                     << ARTIFACT_EVENT_RESOURCE_SHIFT)
                                    | ARTIFACT_EVENT_MODE_RESOURCE_3;
                            else if (gArtifactLevel[IDX(value26)] == ARTIFACT_LEVEL_MAJOR)
                                cell2->m_objectMetadata =
                                    (Random(IDX(RES_WOOD), IDX(RES_GEMS))
                                     << ARTIFACT_EVENT_RESOURCE_SHIFT)
                                    | ARTIFACT_EVENT_MODE_RESOURCE_5;
                        } else {
                            // Retail artifact-tier guardian choice payload.
                            // NOLINTBEGIN(readability-magic-numbers)
                            artifactGuardianChoices[6] = CREATURE_PALADIN;
                            artifactGuardianChoices[7] = CREATURE_CRUSADER;
                            artifactGuardianChoices[8] = CREATURE_CYCLOPS;
                            artifactGuardianChoices[9] = CREATURE_GENIE;
                            artifactGuardianChoices[0] = CREATURE_GREEN_DRAGON;
                            artifactGuardianChoices[1] = CREATURE_RED_DRAGON;
                            artifactGuardianChoices[2] = CREATURE_BLACK_DRAGON;
                            artifactGuardianChoices[3] = CREATURE_BONE_DRAGON;
                            artifactGuardianChoices[4] = CREATURE_GIANT;
                            artifactGuardianChoices[5] = CREATURE_TITAN;
                            // NOLINTEND(readability-magic-numbers)
                            cell2->m_objectMetadata = ARTIFACT_EVENT_GUARDED_FLAG;
                            if (gArtifactLevel[IDX(value26)] == ARTIFACT_LEVEL_TREASURE)
                                cell2->m_objectMetadata |= IDX(CREATURE_ROGUE);
                            else if (gArtifactLevel[IDX(value26)] == ARTIFACT_LEVEL_MINOR)
                                cell2->m_objectMetadata |= IDX(artifactGuardianChoices[Random(
                                    EVENT_ROLL_MIN,
                                    MINOR_GUARDIAN_CHOICE_COUNT - 1
                                ) + MINOR_GUARDIAN_CHOICE_FIRST]);
                            else
                                cell2->m_objectMetadata |= IDX(artifactGuardianChoices[Random(
                                    EVENT_ROLL_MIN,
                                    MAJOR_GUARDIAN_CHOICE_COUNT - 1
                                ) + MAJOR_GUARDIAN_CHOICE_FIRST]);
                        }
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE:
                    mineId2 = GetTownId(xPos2, yPos19);
                    for (row18 = yPos19 - CASTLE_METADATA_TOP_OFFSET;
                         row18 <= yPos19 + CASTLE_METADATA_BOTTOM_OFFSET;
                         row18++) {
                        for (column1 = xPos2 - CASTLE_METADATA_X_RADIUS;
                             column1 <= xPos2 + CASTLE_METADATA_X_RADIUS;
                             column1++) {
                            if (m_worldMap.GetCell(column1, row18)->m_objectMetadata != 0)
                                continue;
                            m_worldMap.GetCell(column1, row18)->m_objectMetadata = mineId2;
                        }
                    }
                    townRec4 = GetCastleSlot(this, mineId2);
                    townRec4->m_boatY = -1;
                    townRec4->m_boatX = townRec4->m_boatY;
                    if (yPos19 <= MAP_HEIGHT - CASTLE_BOAT_Y_OFFSET - 1) {
                        townEntrance = gpAdvManager->GetCell(
                            xPos2 - CASTLE_BOAT_X_OFFSET,
                            yPos19 + CASTLE_BOAT_Y_OFFSET
                        );
                        if (giGroundToTerrain[townEntrance->m_terrainImageIndex]
                            == TERRAIN_WATER) {
                            townRec4->m_boatX = static_cast<i8>(xPos2 - CASTLE_BOAT_X_OFFSET);
                            townRec4->m_boatY = static_cast<i8>(yPos19 + CASTLE_BOAT_Y_OFFSET);
                        } else {
                            townEntrance = gpAdvManager->GetCell(
                                xPos2 + CASTLE_BOAT_X_OFFSET,
                                yPos19 + CASTLE_BOAT_Y_OFFSET
                            );
                            if (giGroundToTerrain[townEntrance->m_terrainImageIndex]
                                == TERRAIN_WATER) {
                                townRec4->m_boatX =
                                    static_cast<i8>(xPos2 + CASTLE_BOAT_X_OFFSET);
                                townRec4->m_boatY =
                                    static_cast<i8>(yPos19 + CASTLE_BOAT_Y_OFFSET);
                            }
                        }
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_LIGHTHOUSE: {
                    m_worldMap.GetCell(xPos2, yPos19)->m_objectMetadata = GetMineId(xPos2, yPos19);
                    break;
                }
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ABANDONED_MINE:
                    mineId2 = GetMineId(xPos2, yPos19);
                    m_mines[mineId2].guardianType = CREATURE_GHOST;
                    m_mines[mineId2].guardianCount = static_cast<u8>(Random(
                        ABANDONED_MINE_GUARDIAN_COUNT_MIN,
                        ABANDONED_MINE_GUARDIAN_COUNT_MAX
                    ));
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE:
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SAWMILL:
                    mineId2 = GetMineId(xPos2, yPos19);
                    for (row18 = yPos19 - MINE_METADATA_TOP_OFFSET; row18 <= yPos19;
                         row18++) {
                        for (column1 = xPos2 - MINE_METADATA_LEFT_OFFSET;
                             column1 <= xPos2 + MINE_METADATA_RIGHT_OFFSET;
                             column1++) {
                            if (column1 == xPos2 - MINE_METADATA_LEFT_OFFSET
                                && cell2->m_triggerType
                                       != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB))
                                continue;
                            if (m_worldMap.GetCell(column1, row18)->m_objectMetadata == 0
                                || ((m_worldMap.GetCell(column1, row18)->m_triggerType
                                     & MAP_TRIGGER_TYPE_MASK)
                                    == (cell2->m_triggerType & MAP_TRIGGER_TYPE_MASK)))
                                m_worldMap.GetCell(column1, row18)->m_objectMetadata = mineId2;
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

    for (yPos19 = 0; yPos19 < MAP_HEIGHT; yPos19++) {
        for (xPos2 = 0; xPos2 < MAP_WIDTH; xPos2++) {
            cell2 = m_worldMap.Row(yPos19) + xPos2;
            if (cell2->m_objectIndex != MAPCELL_SPRITE_NONE && cell2->m_objectLayerBit1) {
                valid27 = 1;
                extraIndex3 = cell2->m_extraIndex;
                while (extraIndex3 != 0) {
                    extra15 = m_worldMap.Extra(extraIndex3);
                    if (extra15->objectIndex != MAPCELL_SPRITE_NONE
                        && !extra15->objectLayerBit1)
                        valid27 = 0;
                    extraIndex3 = extra15->nextIndex;
                }
                if (valid27)
                    cell2->m_flags |= IDX(MAP_CELL_OBJECT_SHADOW_ONLY);
            }
        }
    }

    for (yPos19 = 0; yPos19 < MAP_HEIGHT; yPos19++) {
        for (xPos2 = 0; xPos2 < MAP_WIDTH; xPos2++) {
            cell2 = m_worldMap.Row(yPos19) + xPos2;
            if ((cell2->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_ROCK
                && cell2->m_objectTileset == TILESET_X_LOC2)
                cell2->m_flags |= IDX(MAP_CELL_OCCUPIED);
            if (cell2->m_objectIndex != MAPCELL_SPRITE_NONE
                && !(cell2->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell2->m_flags & IDX(MAP_CELL_OBJECT_SHADOW_ONLY))
                && cell2->m_overlayIndex != MAPCELL_SPRITE_NONE)
                cell2->m_flags |= IDX(MAP_CELL_OCCUPIED);
            upperCount = 0;
            lowerCount16 = 0;
            if (!(cell2->m_flags & IDX(MAP_CELL_OCCUPIED)) && yPos19 < MAP_HEIGHT - 1
                && cell2->m_objectIndex != MAPCELL_SPRITE_NONE
                && !(cell2->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell2->m_flags & IDX(MAP_CELL_OBJECT_SHADOW_ONLY))) {
                mapCell* below0;
                if (m_worldMap.GetCell(xPos2, yPos19 + 1)->m_objectIndex != MAPCELL_SPRITE_NONE
                    && !(
                        m_worldMap.GetCell(xPos2, yPos19 + 1)->m_triggerType
                        & MAP_TRIGGER_ACTION_FLAG
                    )
                    && !(
                        m_worldMap.GetCell(xPos2, yPos19 + 1)->m_flags
                        & IDX(MAP_CELL_OBJECT_SHADOW_ONLY)
                    )) {
                    if (!cell2->m_objectLayerBit1) {
                        upperTilesets29[upperCount] = cell2->m_objectTileset;
                        upperIndexes1[upperCount] = cell2->m_objectIndex;
                        upperCount++;
                    }
                    if (cell2->m_extraIndex != 0)
                        extra15 = m_worldMap.Extra(cell2->m_extraIndex);
                    else
                        extra15 = NULL;
                    while (upperCount < LAYER_SCAN_CAPACITY && extra15 != NULL) {
                        if (extra15->objectIndex != MAPCELL_SPRITE_NONE
                            && !extra15->objectLayerBit1) {
                            upperTilesets29[upperCount] = extra15->objectTileset;
                            upperIndexes1[upperCount] = extra15->objectIndex;
                            upperCount++;
                        }
                        if (extra15->nextIndex != 0)
                            extra15 = m_worldMap.Extra(extra15->nextIndex);
                        else
                            extra15 = NULL;
                    }
                    below0 = m_worldMap.GetCell(xPos2, yPos19 + 1);
                    if (!below0->m_objectLayerBit1) {
                        lowerTilesets4[lowerCount16] = below0->m_objectTileset;
                        lowerIndexes7[lowerCount16] = below0->m_objectIndex;
                        lowerCount16++;
                    }
                    if (below0->m_extraIndex != 0)
                        extra15 = m_worldMap.Extra(below0->m_extraIndex);
                    else
                        extra15 = NULL;
                    while (lowerCount16 < LAYER_SCAN_CAPACITY && extra15 != NULL) {
                        if (extra15->objectIndex != MAPCELL_SPRITE_NONE
                            && !extra15->objectLayerBit1) {
                            lowerTilesets4[lowerCount16] = extra15->objectTileset;
                            lowerIndexes7[lowerCount16] = extra15->objectIndex;
                            lowerCount16++;
                        }
                        if (extra15->nextIndex != 0)
                            extra15 = m_worldMap.Extra(extra15->nextIndex);
                        else
                            extra15 = NULL;
                    }
                    for (randomValue7 = 0; randomValue7 < upperCount; randomValue7++) {
                        for (j9 = 0; lowerCount16 > j9; j9++) {
                            if (lowerTilesets4[j9] == upperTilesets29[randomValue7]
                                || (IsTownObjectTileset(upperTilesets29[randomValue7])
                                    && IsTownObjectTileset(lowerTilesets4[j9])))
                                cell2->m_flags |= IDX(MAP_CELL_OCCUPIED);
                        }
                    }
                }
            }
            if (yPos19 < MAP_HEIGHT - 1) {
                if (m_worldMap.GetCell(xPos2, yPos19 + 1)->m_triggerType
                        == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)
                    || m_worldMap.GetCell(xPos2, yPos19 + 1)->m_triggerType
                           == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_TOWN)
                    || m_worldMap.GetCell(xPos2, yPos19 + 1)->m_triggerType
                           == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_CASTLE))
                    cell2->m_flags |= IDX(MAP_CELL_OCCUPIED);
            }
            if (cell2->m_objectIndex != MAPCELL_SPRITE_NONE
                && !(cell2->m_triggerType & MAP_TRIGGER_ACTION_FLAG)
                && !(cell2->m_flags & IDX(MAP_CELL_OBJECT_SHADOW_ONLY))
                // Bit 2 is authored map data used by retail's occupancy pass. Its broader
                // semantics remain unproven, so do not give it a speculative shared name.
                // NOLINTNEXTLINE(readability-magic-numbers)
                && (yPos19 == MAP_HEIGHT - 1 || (m_worldMap.Row(yPos19 + 1)[xPos2].m_flags & 4)))
                cell2->m_flags |= IDX(MAP_CELL_OCCUPIED);
        }
    }
}

VA(0x00478a49, 0xa1)
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
                if (xPasswordStringsIndex[j] == xPasswordStringsIndex[i])
                    flag = 0;
            }
        }
    }
}

VA(0x00478aea, 0x64)
void game::RandomizeBarrier(mapCell* cell) {
    i32 idx = cell->m_objectMetadata;
    idx &= PASSWORD_INDEX_MASK;
    i32 pass = xPasswordStringsIndex[idx];
    i32 color = (pass << PASSWORD_COLOR_SHIFT) | idx;
    cell->m_objectMetadata = color | 0;
}

VA(0x00478b4e, 0x24)
void game::RandomizePassword(mapCell* cell) {
    RandomizeBarrier(cell);
}

VA(0x00478b72, 0x478)
i32 game::LoadMap(char* filename) {
    char column5[LOAD_MAP_COORDINATE_SCRATCH_SIZE];
    i32 i37;
    i32 file2;
    char row9[LOAD_MAP_COORDINATE_SCRATCH_SIZE];
    char type5[LOAD_MAP_RECORD_SCRATCH_SIZE];
    char trailer15[LOAD_MAP_RECORD_SCRATCH_SIZE];

    sprintf(gText, DATA_COMPGEN(0x004f7278, loadMapSS, "%s%s"), gcMapPath, filename);
    file2 = open(gText, _O_BINARY);
    if (file2 == -1)
        FileError(gText);
    read(file2, &m_mapHeader, sizeof(m_mapHeader));
    m_worldMap.Read(file2, 1);
    SetMapSize(m_worldMap.width, m_worldMap.height);

    for (i37 = 0; i37 < GAME_TOWN_COUNT; i37++) {
        read(file2, column5, sizeof(column5[0]));
        read(file2, row9, sizeof(row9[0]));
        read(file2, type5, sizeof(type5[0]));
        if (static_cast<u8>(column5[0]) != SAVED_TOWN_OFF_MAP) {
            m_castleRecs[i37].m_onMap = 1;
            m_castleRecs[i37].m_x = static_cast<u8>(column5[0]);
            m_castleRecs[i37].m_y = static_cast<u8>(row9[0]);
            m_castleRecs[i37].m_type =
                static_cast<FactionType>(type5[0] & TOWN_RECORD_TYPE_MASK);
            if (type5[0] < 0)
                m_castleRecs[i37].m_buildings |= IDX(TOWN_BUILDING_CASTLE);
            else
                m_castleRecs[i37].m_buildings |= IDX(TOWN_BUILDING_TENT);
        }
    }

    for (i37 = 0; i37 < GAME_MINE_COUNT; i37++) {
        if (m_mapHeader.magic == MAP_HEADER_MAGIC_BASE_GAME && i37 >= GAME_TOWN_COUNT) {
            column5[0] = -1;
            row9[0] = -1;
            type5[0] = -1;
        } else {
            read(file2, column5, sizeof(column5[0]));
            read(file2, row9, sizeof(row9[0]));
            read(file2, type5, sizeof(type5[0]));
        }
        if (static_cast<u8>(column5[0]) != SAVED_TOWN_OFF_MAP) {
            m_mines[i37].guardianType = CREATURE_NONE;
            m_mines[i37].x = static_cast<u8>(column5[0]);
            m_mines[i37].y = static_cast<u8>(row9[0]);
            m_mines[i37].resourceType = static_cast<MineType>(type5[0]);
        }
    }

    m_mapHeader.magic = MAP_HEADER_MAGIC_EXPANSION_GAME;
    read(file2, &m_obeliskCount, sizeof(m_obeliskCount));
    read(
        file2,
        m_rumourEventIndices,
        m_mapHeader.rumourCount * sizeof(m_rumourEventIndices[0])
    );
    m_rumourEventCount = m_mapHeader.rumourCount;
    read(
        file2,
        m_timeEventIndices,
        m_mapHeader.timeEventCount * sizeof(m_timeEventIndices[0])
    );
    m_timeEventCount = m_mapHeader.timeEventCount;
    read(file2, &iMaxMapExtra, sizeof(iMaxMapExtra));
    ppMapExtra = reinterpret_cast<void**>(
        H2_ALLOC_AT(
            iMaxMapExtra * sizeof(ppMapExtra[0]),
            DATA_COMPGEN(0x004f7280, loadMapSourceFile, RETAIL_FILE),
            gMapSourceLine + 0x59
        )
    );
    pwSizeOfMapExtra = reinterpret_cast<i16*>(
        H2_ALLOC_AT(
            iMaxMapExtra * sizeof(pwSizeOfMapExtra[0]),
            DATA_COMPGEN(0x004f72a8, loadMapSourceFile2, RETAIL_FILE),
            gMapSourceLine + 0x5a
        )
    );
    memset(ppMapExtra, 0, iMaxMapExtra * sizeof(ppMapExtra[0]));
    memset(pwSizeOfMapExtra, 0, iMaxMapExtra * sizeof(pwSizeOfMapExtra[0]));
    for (i37 = 1; (&i37)[0] < iMaxMapExtra; i37++) {
        read(file2, pwSizeOfMapExtra + i37, sizeof(pwSizeOfMapExtra[0]));
        ppMapExtra[i37] = H2_ALLOC_AT(
            pwSizeOfMapExtra[i37],
            DATA_COMPGEN(0x004f72d0, loadMapSourceFile3, RETAIL_FILE),
            gMapSourceLine + 0x62
        );
        read(file2, ppMapExtra[i37], pwSizeOfMapExtra[i37]);
    }
    read(file2, trailer15, sizeof(u16));
    close(file2);
    return 0;
}

VA(0x00478fea, 0x3aa)
void game::ClaimTown(i32 townId, i32 player, i32 suppressVisibility) {
    i32 i;
    town* townRec;
    mapCell* cellPtr;

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
    townRec->m_formation = 0;
    if (m_castleOwners[townId] != -1)
        GetCastle(townId)->Deallocate();
    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        townRec->m_army.m_creatureTypes[i] = CREATURE_NONE;
        townRec->m_army.m_creatureCounts[i] = 0;
    }
    m_castleRecs[townId].m_turnsOwned =
        m_castleRecs[townId].m_owner == -1 ? TOWN_NEW_OWNER_TURN_COUNT : 0;
    m_castleRecs[townId].m_owner = static_cast<i8>(player);
    m_castleOwners[townId] = static_cast<i8>(player);
    m_players[player].m_townIds[m_players[player].m_townCount] = static_cast<i8>(townId);
    m_players[player].m_townCount++;

    cellPtr = m_worldMap.GetCell(m_castleRecs[townId].m_x - 1, m_castleRecs[townId].m_y);
    m_worldMap.ChangeTilesetIndex(
        cellPtr,
        m_castleRecs[townId].m_x - 1,
        m_castleRecs[townId].m_y,
        TILESET_FLAG32,
        GetPlayerColor(static_cast<i8>(player)) * TOWN_FLAG_FRAME_STRIDE,
        1,
        -1
    );
    cellPtr = m_worldMap.GetCell(m_castleRecs[townId].m_x + 1, m_castleRecs[townId].m_y);
    m_worldMap.ChangeTilesetIndex(
        cellPtr,
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

VA(0x00479394, 0x4c2)
void game::ClaimMine(i32 mineId, i32 player) {
    mapCell* acc;
    i32 flag;
    i32 x;
    i32 y;

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
    acc = m_worldMap.Row(y) + x;
    if (player == -1) {
        m_worldMap.ChangeTilesetIndex(
            acc, x, y, TILESET_FLAG32, MAPCELL_SPRITE_NONE, 1, -1
        );
    } else {
        m_worldMap.ChangeTilesetIndex(
            acc,
            x,
            y,
            TILESET_FLAG32,
            GetPlayerColor(static_cast<i8>(player)) + flag,
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

VA(0x00479856, 0x1e2)
SpellType
game::ViewSpells(
    hero* spellHero, HeroSpellType spellType, MessageDispatchHandler callback, i32 readOnly
) {
    tag_message message;

    viewSpellsHero = spellHero;
    m_viewSpell = SPELL_NONE;
    if (spellHero->GetNumSpells(spellType) == 0) {
        NormalDialog(const_cast<char*>(DATA_COMPGEN(0x004f72f8, viewSpellsNoSpellsToCast, "No spells to cast.")), 1, -1, -1, -1, 0, -1, 0, -1, 0);
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
            VIEW_SPELLS_WINDOW_X, VIEW_SPELLS_WINDOW_Y, const_cast<char*>(DATA_COMPGEN(0x004f730c, viewSpellsSpellwinBin, "spellwin.bin"))
        );
        if (m_viewSpellsWindow == NULL)
            MemError();
        if (spellType != SPELL_TYPE_ALL) {
            message.type = MESSAGE_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            if (spellType == SPELL_TYPE_COMBAT)
                message.payload.widget.id = VIEW_SPELL_COMBAT_TAB_ID;
            else
                message.payload.widget.id = VIEW_SPELL_ADVENTURE_TAB_ID;
            message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
            m_viewSpellsWindow->BroadcastMessage(message);
        }
        UpdateSpellWidgets();
        gpWindowManager->DoDialog(m_viewSpellsWindow, ViewSpellsHandler, 0);
        delete m_viewSpellsWindow;
    }
    return m_viewSpell;
}

VA(0x00479a38, 0x403)
void game::UpdateSpellWidgets(void) {
    tag_message message9;
    i32 spellSlot6;
    i32 spellPoints0;
    SpellType spell2;
    i32 lineLength0;

    message9.type = MESSAGE_WIDGET;
    spellPoints0 = m_viewSpellsHero->m_spellPoints;
    if (spellPoints0 > VIEW_SPELL_MANA_MAX)
        spellPoints0 = VIEW_SPELL_MANA_MAX;
    message9.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
    if (spellPoints0 > VIEW_SPELL_MANA_HUNDREDS_THRESHOLD)
        message9.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
    else
        message9.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message9.payload.widget.id = VIEW_SPELL_MANA_HUNDREDS_ID;
    m_viewSpellsWindow->BroadcastMessage(message9);
    if (spellPoints0 > VIEW_SPELL_MANA_TENS_THRESHOLD)
        message9.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
    else
        message9.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message9.payload.widget.id = VIEW_SPELL_MANA_TENS_ID;
    m_viewSpellsWindow->BroadcastMessage(message9);

    sprintf(
        gText,
        DATA_COMPGEN(0x004f731c, updateSpellWidgetsD, "%d"),
        (spellPoints0 / VIEW_SPELL_MANA_HUNDREDS_DIVISOR) % VIEW_SPELL_MANA_DIGIT_BASE
    );
    message9.payload.widget.data.text = gText;
    message9.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message9.payload.widget.id = VIEW_SPELL_MANA_HUNDREDS_ID;
    m_viewSpellsWindow->BroadcastMessage(message9);
    sprintf(
        gText,
        DATA_COMPGEN(0x004f7320, updateSpellWidgetsD2, "%d"),
        (spellPoints0 / VIEW_SPELL_MANA_TENS_DIVISOR) % VIEW_SPELL_MANA_DIGIT_BASE
    );
    message9.payload.widget.data.text = gText;
    message9.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message9.payload.widget.id = VIEW_SPELL_MANA_TENS_ID;
    m_viewSpellsWindow->BroadcastMessage(message9);
    sprintf(gText, DATA_COMPGEN(0x004f7324, updateSpellWidgetsD3, "%d"), spellPoints0 % VIEW_SPELL_MANA_DIGIT_BASE);
    message9.payload.widget.data.text = gText;
    message9.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message9.payload.widget.id = VIEW_SPELL_MANA_ONES_ID;
    m_viewSpellsWindow->BroadcastMessage(message9);

    for (spellSlot6 = 0; spellSlot6 < VIEW_SPELL_PAGE_SIZE; spellSlot6++) {
        if (m_viewSpellsTop[IDX(m_viewSpellsType)] + spellSlot6
            >= m_viewSpellsCount[IDX(m_viewSpellsType)]) {
            message9.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message9.payload.widget.id = spellSlot6 + VIEW_SPELL_ICON_ID_BASE;
            message9.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
            m_viewSpellsWindow->BroadcastMessage(message9);
            message9.payload.widget.id = spellSlot6 + VIEW_SPELL_TEXT_ID_BASE;
            m_viewSpellsWindow->BroadcastMessage(message9);
        } else {
            spell2 = m_viewSpellsHero->GetNthSpell(
                m_viewSpellsType,
                m_viewSpellsTop[IDX(m_viewSpellsType)] + spellSlot6 + 1
            );
            message9.payload.widget.command = WIDGET_COMMAND_SET_FILL_COLOR;
            message9.payload.widget.id = spellSlot6 + VIEW_SPELL_TEXT_ID_BASE;
            if (GetManaCost(spell2, m_viewSpellsHero) > m_viewSpellsHero->m_spellPoints)
                message9.payload.widget.data.value = VIEW_SPELL_UNAVAILABLE_COLOR;
            else
                message9.payload.executive.command = EXECUTIVE_COMMAND_TERMINATE_LOOP;
            m_viewSpellsWindow->BroadcastMessage(message9);
            message9.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message9.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
            m_viewSpellsWindow->BroadcastMessage(message9);
            message9.payload.widget.id = spellSlot6 + VIEW_SPELL_ICON_ID_BASE;
            m_viewSpellsWindow->BroadcastMessage(message9);
            if (m_viewSpellsReadOnly) {
                message9.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
                message9.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED);
                m_viewSpellsWindow->BroadcastMessage(message9);
            }
            message9.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message9.payload.widget.data.value = gsSpellInfo[IDX(spell2)].iconIndex;
            m_viewSpellsWindow->BroadcastMessage(message9);
            lineLength0 = smallFont->LineLength(gSpellNames[IDX(spell2)], VIEW_SPELL_NAME_WIDTH);
            if (lineLength0 == 1) {
                sprintf(
                    gText,
                    DATA_COMPGEN(0x004f7328, updateSpellWidgetsSD, "%s\n[%d]"),
                    gSpellNames[IDX(spell2)],
                    GetManaCost(spell2, m_viewSpellsHero)
                );
            } else {
                sprintf(
                    gText,
                    DATA_COMPGEN(0x004f7330, updateSpellWidgetsSD2, "%s [%d]"),
                    gSpellNames[IDX(spell2)],
                    GetManaCost(spell2, m_viewSpellsHero)
                );
            }
            message9.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            message9.payload.widget.id = spellSlot6 + VIEW_SPELL_TEXT_ID_BASE;
            message9.payload.widget.data.text = gText;
            m_viewSpellsWindow->BroadcastMessage(message9);
        }
    }
}

VA(0x00479e3b, 0x692)
MessageDispatchResult ViewSpellsHandler(tag_message& msg) {
    SpellType spell;

    if (msg.type == MESSAGE_MOUSE_MOVE) {
        gpWindowManager->ConvertToHover(msg);
        if (msg.payload.hover.id == gpWindowManager->m_lastHoverId) {
            return MESSAGE_DISPATCH_CONSUME;
        } else {
            return gpGame->m_viewSpellsCallback(msg);
        }
    }
    if (msg.type == MESSAGE_WIDGET) {
        switch (msg.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                if (msg.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT
                    || (HAS(msg.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) != 0)
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
                            if (gpGame->m_viewSpellsTop[IDX(gpGame->m_viewSpellsType)] == 0)
                                break;
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
                            msg.payload.widget.id = VIEW_SPELL_CLOSE_ID;
                            break;
                    }
                }
                break;
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                if (msg.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT
                    || (HAS(msg.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) != 0) {
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
                                gpGame->m_viewSpellsTop[IDX(gpGame->m_viewSpellsType)]
                                    + (msg.payload.widget.id - VIEW_SPELL_ICON_ID_BASE) + 1
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
                                gpGame->m_viewSpellsTop[IDX(gpGame->m_viewSpellsType)]
                                    + (msg.payload.widget.id - VIEW_SPELL_ICON_ID_BASE) + 1
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
                                    DATA_COMPGEN(0x004f7338, viewSpellsHandlerThatSpellCostsDManaYou, "That spell costs %d mana.  You only have %d mana, so you "
                                    "can't cast the spell."),
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

VA(0x0047a4cd, 0x17c)
MessageDispatchResult ViewSpecialHandler(tag_message& msg) {
    if (msg.type == MESSAGE_MOUSE_MOVE) {
        if (gpWindowManager->m_lastHoverId == msg.payload.hover.id)
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

VA(0x0047a649, 0xc86)
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
    DATA(0x004f7388) static i16 viewArmySourceLineBase = 0x0dd1;
    // Retail initializes these unused UI locals; retaining them preserves the /Od frame.
    i16 baseX7 = VIEW_ARMY_UNUSED_BASE_X;
    i16 quickBaseY3 = VIEW_ARMY_UNUSED_QUICK_BASE_Y;
    i16 blankWidget3 = VIEW_ARMY_BLANK_WIDGET_ID;
    i16 numWidget15 = VIEW_ARMY_COUNT_WIDGET_ID;
    i16 titleMessage15 = VIEW_ARMY_TITLE_WIDGET_ID;
    i16 detailMessage2 = VIEW_ARMY_DETAIL_WIDGET_ID;
    i16 frame18 = VIEW_ARMY_MONSTER_WIDGET_ID;
    i32 loopIndex0;
    tag_message message6;
    message6.type = MESSAGE_WIDGET;

    iViewArmyFrame = 0;
    iViewArmyType = monsterType;
    iViewArmyNumTroops = numTroops;
    gbAllowUpgrade = false;

    if (castle && (gpAdvManager->m_active == 1 || gpTownManager->m_active == 1)) {
        for (loopIndex0 = IDX(BUILDING_SLOT_DWELLING_SECOND);
             loopIndex0 <= IDX(BUILDING_SLOT_DWELLING_SIXTH);
             loopIndex0++) {
            if (gDwellingType[IDX(castle->m_type)]
                             [loopIndex0 - IDX(BUILDING_SLOT_DWELLING_FIRST)]
                    == monsterType
                && (castle->m_buildings
                    & (1 << (loopIndex0 + VIEW_ARMY_DWELLING_UPGRADE_OFFSET)))) {
                gbAllowUpgrade = true;
                iViewArmyUpgradeToType = NextCreatureType(monsterType);
            }
        }
        if ((monsterType == CREATURE_GREEN_DRAGON || monsterType == CREATURE_RED_DRAGON)
            && (castle->m_buildings & IDX(KB_DWELLING_UPGRADE_SIXTH_FLAG))) {
            gbAllowUpgrade = true;
            iViewArmyUpgradeToType = CREATURE_BLACK_DRAGON;
        }
    }

    tag_monsterInfo* monster8 = &gMonsterDatabase[IDX(monsterType)];
    tag_monsterInfo* armyMonster11;
    if (theArmy)
        armyMonster11 = &theArmy->m_monster;
    else
        armyMonster11 = &gMonsterDatabase[IDX(monsterType)];

    x = VIEW_ARMY_WINDOW_X;
    y = VIEW_ARMY_WINDOW_Y;
    m_viewArmyWindow = new heroWindow(x, y, const_cast<char*>(DATA_COMPGEN(0x004f738c, viewArmyArmywinBin, "armywin.bin")));
    if (!m_viewArmyWindow)
        MemError();

    viewArmyFacingWIPXMod = facing == ARMY_FACING_RIGHT ? -1 : 1;
    gpResourceManager
        ->PointToFile(gpResourceManager->MakeId(cArmyFrameFileNames[IDX(monsterType)], 1));
    gpResourceManager->ReadBlock(
        reinterpret_cast<i8*>(&sViewArmyMonFrameInfo),
        sizeof(sViewArmyMonFrameInfo)
    );
    ModifyFrameInfo(&sViewArmyMonFrameInfo, monsterType);
    BuildTempWalkSeq(&sViewArmyMonFrameInfo, 0, 1);

    viewArmyBaseX = VIEW_ARMY_MONSTER_BASE_X;
    char filename4[VIEW_ARMY_FILENAME_SIZE];
    if (gbLowMemory)
        sprintf(filename4, DATA_COMPGEN(0x004f7398, viewArmyMonh04dIcn, "monh%04d.icn"), IDX(monsterType));
    else
        strcpy(filename4, cMonFilename[IDX(monsterType)]);

    icon* monsterIcon5 = gpResourceManager->GetIcon(filename4);
    i32 iconFrame15 = sViewArmyMonFrameInfo.animationFrames[IDX(ARMY_ANIMATION_WALK)][0];
    viewArmyBaseX += (GetIconEntry(monsterIcon5, iconFrame15)->w
                      / VIEW_ARMY_ICON_CENTER_DIVISOR)
                     * viewArmyFacingWIPXMod;
    viewArmyBaseX += GetIconEntry(monsterIcon5, iconFrame15)->x * viewArmyFacingWIPXMod
                     + sViewArmyMonFrameInfo.walkXOffsets[0] * viewArmyFacingWIPXMod;
    viewArmyBaseY = VIEW_ARMY_MONSTER_BASE_Y;
    viewArmyBaseY +=
        GetIconEntry(monsterIcon5, iconFrame15)->h / VIEW_ARMY_ICON_CENTER_DIVISOR;
    if (gbLowMemory) {
        viewArmyBaseX = VIEW_ARMY_LOW_MEMORY_MONSTER_X;
        viewArmyBaseY = VIEW_ARMY_LOW_MEMORY_MONSTER_Y;
    }

    iconWidget* monsterWidget7 = new iconWidget(
        static_cast<i16>(viewArmyBaseX),
        static_cast<i16>(viewArmyBaseY),
        VIEW_ARMY_MONSTER_WIDGET_WIDTH,
        VIEW_ARMY_MONSTER_WIDGET_HEIGHT,
        filename4,
        gbLowMemory ? 0 : sViewArmyMonFrameInfo.animationFrames[IDX(ARMY_ANIMATION_WALK)][0],
        facing == ARMY_FACING_LEFT ? ICON_DRAW_FLIPPED : ICON_DRAW_NORMAL,
        VIEW_ARMY_MONSTER_WIDGET_Z_ORDER,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (!monsterWidget7)
        MemError();
    m_viewArmyWindow->AddWidget(monsterWidget7, -1);
    gpResourceManager->Dispose(monsterIcon5);

    char armyName8[VIEW_ARMY_NAME_SIZE];
    strcpy(armyName8, gArmyNames[IDX(monsterType)]);
    armyName8[0] -= VIEW_ARMY_ASCII_CASE_OFFSET;
    message6.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message6.payload.widget.id = VIEW_ARMY_TITLE_WIDGET_ID;
    message6.payload.widget.data.text = armyName8;
    m_viewArmyWindow->BroadcastMessage(message6);

    char* details9 = static_cast<char*>(H2_ALLOC_AT(VIEW_ARMY_DETAIL_BUFFER_SIZE, DATA_COMPGEN(0x004f73a8, viewArmySourceFile, RETAIL_FILE), viewArmySourceLineBase + 0x93));
    i32 morale2 = theGroup ? theGroup->GetMorale(theHero, castle, NULL) : 0;
    if (HAS(monster8->flags.all, MONSTER_FLAGS_NO_MORALE))
        morale2 = 0;

    sprintf(details9, DATA_COMPGEN(0x004f73d0, viewArmyEmptyString, ""));
    i32 modifier15 = 0;
    sprintf(
        gText,
        DATA_COMPGEN(0x004f73d4, viewArmySD, "%s%d"),
        cArmyDetail[ARMY_DETAIL_ATTACK],
        static_cast<i32>(monster8->attack)
    );
    strcat(details9, gText);
    if (theHero)
        modifier15 += theHero->Stats(HERO_PRIMARY_ATTACK);
    if (theArmy)
        modifier15 = theArmy->m_monster.attack - monster8->attack;
    if (modifier15) {
        sprintf(gText, DATA_COMPGEN(0x004f73dc, viewArmyD, " (%d)"), monster8->attack + modifier15);
        strcat(details9, gText);
    }

    modifier15 = 0;
    sprintf(
        gText,
        DATA_COMPGEN(0x004f73e4, viewArmySD2, "\n%s%d"),
        cArmyDetail[ARMY_DETAIL_DEFENSE],
        static_cast<i32>(monster8->defense)
    );
    strcat(details9, gText);
    if (theHero)
        modifier15 += theHero->Stats(HERO_PRIMARY_DEFENSE);
    if (theArmy)
        modifier15 = theArmy->m_monster.defense - monster8->defense;
    if (modifier15) {
        sprintf(gText, DATA_COMPGEN(0x004f73ec, viewArmyD2, " (%d)"), monster8->defense + modifier15);
        strcat(details9, gText);
    }

    if (HAS(monster8->flags.all, MONSTER_FLAGS_SHOOTER)) {
        i32 shots8 = armyMonster11->shots;
        if (shots8 > 0) {
            if (gpCombatManager->m_active == 1)
                sprintf(gText, DATA_COMPGEN(0x004f73f4, viewArmySD3, "\n%s%d"), cArmyDetail[ARMY_DETAIL_SHOTS_LEFT], shots8);
            else
                sprintf(gText, DATA_COMPGEN(0x004f73fc, viewArmySD4, "\n%s%d"), cArmyDetail[ARMY_DETAIL_SHOTS_OUTSIDE], shots8);
            strcat(details9, gText);
        }
    }

    sprintf(
        gText,
        DATA_COMPGEN(0x004f7404, viewArmySD5, "\n%s%d"),
        cArmyDetail[ARMY_DETAIL_DAMAGE],
        static_cast<i32>(monster8->damageMin)
    );
    strcat(details9, gText);
    if (monster8->damageMin != monster8->damageMax) {
        sprintf(gText, DATA_COMPGEN(0x004f740c, viewArmyD3, "-%d"), static_cast<i32>(monster8->damageMax));
        strcat(details9, gText);
    }
    sprintf(
        gText,
        DATA_COMPGEN(0x004f7410, viewArmySD6, "\n%s%d"),
        cArmyDetail[ARMY_DETAIL_HIT_POINTS],
        static_cast<u32>(monster8->hitPoints)
    );
    strcat(details9, gText);
    if (gpCombatManager->m_active == 1) {
        sprintf(
            gText,
            DATA_COMPGEN(0x004f742c, viewArmySD7, "\n%s%d"),
            DATA_COMPGEN(0x004f7418, viewArmyHitPointsLeft, "Hit Points Left: "),
            static_cast<u32>(monster8->hitPoints) - theArmy->m_hitPointsLost
        );
        strcat(details9, gText);
    }
    sprintf(gText, DATA_COMPGEN(0x004f7434, viewArmySS, "\n%s%s"), cArmyDetail[ARMY_DETAIL_SPEED], speedText[armyMonster11->speed]);
    strcat(details9, gText);
    sprintf(
        gText,
        DATA_COMPGEN(0x004f743c, viewArmySS2, "\n%s%s"),
        cArmyDetail[ARMY_DETAIL_MORALE],
        gMoraleText[morale2 + VIEW_ARMY_TEXT_NEUTRAL_OFFSET]
    );
    strcat(details9, gText);
    i32 luck1 = GetLuck(theHero, theArmy, castle);
    sprintf(
        gText,
        DATA_COMPGEN(0x004f7444, viewArmySS3, "\n%s%s"),
        cArmyDetail[ARMY_DETAIL_LUCK],
        gLuckText[luck1 + VIEW_ARMY_TEXT_NEUTRAL_OFFSET]
    );
    strcat(details9, gText);

    message6.payload.widget.id = VIEW_ARMY_DETAIL_WIDGET_ID;
    message6.payload.widget.data.text = details9;
    m_viewArmyWindow->BroadcastMessage(message6);
    if (!gbAllowUpgrade) {
        message6.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message6.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message6.payload.widget.id = VIEW_ARMY_UPGRADE_ACTION_ID;
        m_viewArmyWindow->BroadcastMessage(message6);
    }
    if (disableUpgrade) {
        message6.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message6.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message6.payload.widget.id = VIEW_ARMY_UPGRADE_ID;
        m_viewArmyWindow->BroadcastMessage(message6);
    }
    if (quickView) {
        message6.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message6.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message6.payload.widget.id = VIEW_ARMY_QUICK_VIEW_ID;
        m_viewArmyWindow->BroadcastMessage(message6);
    }
    if (numTroops < 1) {
        message6.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message6.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        message6.payload.widget.id = VIEW_ARMY_BLANK_WIDGET_ID;
        m_viewArmyWindow->BroadcastMessage(message6);
        message6.payload.widget.id = VIEW_ARMY_COUNT_WIDGET_ID;
        m_viewArmyWindow->BroadcastMessage(message6);
    } else {
        char countText[VIEW_ARMY_COUNT_TEXT_SIZE];
        sprintf(countText, DATA_COMPGEN(0x004f744c, viewArmyD4, "%d"), numTroops);
        message6.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message6.payload.widget.id = VIEW_ARMY_COUNT_WIDGET_ID;
        message6.payload.widget.data.text = countText;
        m_viewArmyWindow->BroadcastMessage(message6);
    }

    if (theArmy) {
        i32 spellY3 = VIEW_ARMY_SPELL_BASE_Y;
        i32 spellCenterX8 = VIEW_ARMY_SPELL_CENTER_X;
        if (quickView)
            spellY3 += VIEW_ARMY_QUICK_SPELL_Y_OFFSET;
        i32 spacing0 = VIEW_ARMY_SPELL_SPACING_BASE - theArmy->m_spellCount;
        i32 spellX3 =
            VIEW_ARMY_SPELL_X_BIAS - theArmy->m_spellCount + spellCenterX8
            - (theArmy->m_spellCount * spacing0) / VIEW_ARMY_ICON_CENTER_DIVISOR;
        H2_ENUM_STORAGE_STEPPED(ArmySpellInfluence, i32) spellIndex9 =
            ARMY_SPELL_INFLUENCE_NONE;
        for (loopIndex0 = 0;
             loopIndex0 < (theArmy->m_spellCount < VIEW_ARMY_SPELL_VISIBLE_LIMIT
                               ? theArmy->m_spellCount
                               : VIEW_ARMY_SPELL_VISIBLE_LIMIT);
             loopIndex0++) {
            spellIndex9++;
            for (; spellIndex9 < ARMY_SPELL_INFLUENCE_COUNT; spellIndex9++) {
                if (theArmy->m_spellInfluence[IDX(spellIndex9)])
                    break;
            }
            iconWidget* spellWidget = new iconWidget(
                static_cast<i16>((&loopIndex0)[0] * spacing0 + spellX3),
                static_cast<i16>(spellY3 + VIEW_ARMY_SPELL_WIDGET_Y_OFFSET),
                0,
                0,
                const_cast<char*>(DATA_COMPGEN(0x004f7450, spellWidgetSpellinlIcn, "spellinl.icn")),
                static_cast<i16>(IDX(spellIndex9)),
                ICON_DRAW_NORMAL,
                static_cast<i16>(loopIndex0 + VIEW_ARMY_SPELL_WIDGET_ID_BASE),
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
    H2_FREE_AT(details9, DATA_COMPGEN(0x004f7460, viewArmySourceFile2, RETAIL_FILE), viewArmySourceLineBase + 0x164);
    delete m_viewArmyWindow;
}

VA(0x0047b2cf, 0x3f5)
MessageDispatchResult ViewArmyHandler(tag_message& msg) {
    i32 goldCost6;
    ResourceType resourceType0;
    i32 resourceCost5;

    gbDismissArmy = false;
    gbUpgradeArmy = false;
    i16 frameDelay0 = VIEW_ARMY_HANDLER_FRAME_DELAY;
    i16 frameOffset1;

    if (msg.type == MESSAGE_WIDGET) {
        switch (msg.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                switch (msg.payload.widget.id) {
                    case EVENT_WINDOW_FIRST_BUTTON:
                    case EVENT_WINDOW_SECOND_BUTTON:
                        gpWindowManager->m_dialogResult = msg.payload.widget.id;
                        msg.payload.widget.id = VIEW_ARMY_CLOSE_ID;
                        msg.payload.widget.command = BaseWidgetCommand(msg.payload.widget.id);
                        return MESSAGE_DISPATCH_FORWARD;
                    case EVENT_WINDOW_FOURTH_BUTTON:
                        NormalDialog(
                            const_cast<char*>(DATA_COMPGEN(0x004f7488, viewArmyHandlerAreYouSureYouWantTo, "Are you sure you want to dismiss this army?")),
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
                            msg.payload.widget.command = BaseWidgetCommand(msg.payload.widget.id);
                            return MESSAGE_DISPATCH_FORWARD;
                        }
                        break;
                    case VIEW_ARMY_UPGRADE_ACTION_ID:
                        goldCost6 = (gMonsterDatabase[IDX(iViewArmyUpgradeToType)].cost
                                     - gMonsterDatabase[IDX(iViewArmyType)].cost)
                                    * iViewArmyNumTroops * VIEW_ARMY_UPGRADE_COST_MULTIPLIER;
                        if (iViewArmyUpgradeToType == CREATURE_BLACK_DRAGON) {
                            resourceType0 = RES_SULFUR;
                            resourceCost5 =
                                iViewArmyNumTroops * VIEW_ARMY_UPGRADE_COST_MULTIPLIER;
                        } else if (iViewArmyUpgradeToType == CREATURE_TITAN) {
                            resourceType0 = RES_GEMS;
                            resourceCost5 =
                                iViewArmyNumTroops * VIEW_ARMY_UPGRADE_COST_MULTIPLIER;
                        } else {
                            resourceType0 = RES_NONE;
                            resourceCost5 = 0;
                        }
                        if (gpCurPlayer->m_resources[IDX(RES_GOLD)] >= goldCost6
                            && (resourceType0 == RES_NONE
                                || gpCurPlayer->m_resources[IDX(resourceType0)] >= resourceCost5)) {
                            NormalDialog(
                                const_cast<char*>(
                                    DATA_COMPGEN(0x004f74b4, viewArmyHandlerYourTroopsCanBeUpgradedBut, "Your troops can be upgraded, but it will cost you dearly.  "
                                    "Do you wish to upgrade them?")
                                ),
                                NORMAL_DIALOG_CONFIRM,
                                -1,
                                -1,
                                IDX(RES_GOLD),
                                goldCost6,
                                IDX(resourceType0),
                                resourceCost5,
                                -1,
                                0
                            );
                            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
                                gpCurPlayer->m_resources[IDX(RES_GOLD)] -= goldCost6;
                                if (resourceType0 != RES_NONE)
                                    gpCurPlayer->m_resources[IDX(resourceType0)] -= resourceCost5;
                                gbUpgradeArmy = true;
                                msg.payload.widget.id = VIEW_ARMY_CLOSE_ID;
                                msg.payload.widget.command =
                                    BaseWidgetCommand(msg.payload.widget.id);
                                return MESSAGE_DISPATCH_FORWARD;
                            }
                        } else {
                            NormalDialog(
                                const_cast<char*>(DATA_COMPGEN(0x004f750c, viewArmyHandlerYouCanTAffordToUpgrade, "You can't afford to upgrade your troops!")),
                                NORMAL_DIALOG_INFO,
                                -1,
                                -1,
                                IDX(RES_GOLD),
                                goldCost6,
                                IDX(resourceType0),
                                resourceCost5,
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

    if (!gbLowMemory && KBTickCount() > glTimers[0]) {
        msg.type = MESSAGE_WIDGET;
        msg.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        msg.payload.widget.id = VIEW_ARMY_MONSTER_WIDGET_ID;
        iViewArmyFrame = (iViewArmyFrame + 1)
                         % sViewArmyMonFrameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)];
        msg.payload.widget.data.value =
            sViewArmyMonFrameInfo.animationFrames[IDX(ARMY_ANIMATION_WALK)][iViewArmyFrame];
        gpGame->m_viewArmyWindow->BroadcastMessage(msg);
        msg.payload.widget.command = WIDGET_COMMAND_SET_X;
        msg.payload.widget.data.value =
            sViewArmyMonFrameInfo.walkXOffsets[iViewArmyFrame] * viewArmyFacingWIPXMod
            + viewArmyBaseX;
        gpGame->m_viewArmyWindow->BroadcastMessage(msg);
        gpGame->m_viewArmyWindow->DrawWindow(1, 0, WINDOW_DRAW_ID_LIMIT);
        glTimers[0] = static_cast<i32>(
            KBTickCount()
            + sViewArmyMonFrameInfo.walkDuration * GAME_VIEW_ARMY_FRAME_DELAY_SCALE
                  / sViewArmyMonFrameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK)]
        );
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0047b6c4, 0x671)
i32 game::GetRandomNumTroops(H2_ENUM_PARAM(CreatureType, i32) monsterType) {
    // Retail per-creature neutral-stack ranges. The paired bounds are balance payload,
    // while the switch labels identify the domain that owns each pair.
    // NOLINTBEGIN(readability-magic-numbers)
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
    // NOLINTEND(readability-magic-numbers)
}

VA(0x0047bd35, 0x3f)
void game::TurnOnAIMusic(void) {
    gpSoundManager->StopAllSamples(1);
    gpSoundManager->SwitchAmbientMusic(GAME_AI_MUSIC_TRACK);
    gpSoundManager->m_samplesReady = 0;
}

VA(0x0047bd74, 0x25)
void game::TurnOffAIMusic(void) {
    gpSoundManager->m_samplesReady = 1;
}

VA(0x0047bd99, 0x596)
void game::NextPlayer(void) {
    char scratch[NEXT_PLAYER_SCRATCH_SIZE];
    i32 remotePlayer;
    i32 index;
    i32 humansLeft;

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
        humansLeft = 0;
        for (index = 0; index < GAME_PLAYER_COUNT; index++) {
            if (m_playerDead[index] == 0 && gbHumanPlayer[index])
                humansLeft++;
        }
        SaveGame(const_cast<char*>(DATA_COMPGEN(0x004f7538, nextPlayerAUTOSAVE, "AUTOSAVE")), 1, 0);
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
        SetNoDialogMenus(0);
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
        SetNoDialogMenus(1);
        gpInputManager->Flush();
        gbAllBlack = true;
        gpAdvManager->CheckSetEvilInterface(1, giCurPlayer);
        gbAllBlack = false;
        if (gbBlackoutPlayer && giNumHumanPlayers > 1) {
            sprintf(gText, DATA_COMPGEN(0x004f7544, nextPlayerSSTurn, "%s's turn."), cPlayerNames[giCurPlayer]);
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
        gpSoundManager->m_samplesReady = 0;
    }
    if (m_day == 1 && giCurTurn != 1)
        gpSoundManager->m_samplesReady = 0;

    DoNewTurn();
    CheckEndGame(END_GAME_FORCE_NONE, false);
    if (gbThisNetHumanPlayer[giCurPlayer] && gpSoundManager->m_samplesReady == 0
        && giForceSwitchMusic == -1) {
        gpSoundManager->m_samplesReady = 1;
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

VA(0x0047c32f, 0x432)
i32 game::ComputeDailyGold(i32 player) {
    i32 heroIndex;
    i32 gold = 0;
    i32 index;

    for (index = 0; index < GAME_MINE_COUNT; index++) {
        if (m_mines[index].owner == player) {
            if (m_mines[index].resourceType == MINE_TYPE_GOLD)
                gold += DAILY_GOLD_MINE_INCOME;
            if (m_mines[index].resourceType == MINE_TYPE_ALCHEMIST_LAB)
                gold += DAILY_GOLD_MINE_INCOME;
        }
    }

    for (index = 0; index < GAME_TOWN_COUNT; index++) {
        if (m_castleRecs[index].m_owner == player) {
            if (m_castleRecs[index].m_buildings & BIT(BUILDING_SLOT_UPGRADE_CASTLE))
                gold += DAILY_GOLD_VILLAGE_INCOME;
            else
                gold += DAILY_GOLD_TOWN_INCOME;
            if (m_castleRecs[index].m_buildings & BIT(BUILDING_SLOT_SPECIAL_SEVEN))
                gold += DAILY_GOLD_STATUE_INCOME;
            if (m_castleRecs[index].m_type == FACTION_WARLOCK
                && (m_castleRecs[index].m_buildings & BIT(BUILDING_SLOT_SPECIAL)))
                gold += DAILY_GOLD_DUNGEON_INCOME;
        }
    }

    gold += m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_SACK_GOLD)
            * DAILY_GOLD_ENDLESS_SACK_INCOME;
    gold += m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_BAG_GOLD)
            * DAILY_GOLD_ENDLESS_BAG_INCOME;
    gold += m_players[player].NumOfGivenArtifact(ARTIFACT_ENDLESS_PURSE_GOLD)
            * DAILY_GOLD_ENDLESS_PURSE_INCOME;
    gold += m_players[player].NumOfGivenArtifact(ARTIFACT_GOLDEN_GOOSE)
            * DAILY_GOLD_GOLDEN_GOOSE_INCOME;
    gold += m_players[player].NumOfGivenArtifact(ARTIFACT_TAX_LIEN)
            * DAILY_GOLD_TAX_LIEN_INCOME;

    for (heroIndex = 0; heroIndex < m_players[player].m_heroCount; heroIndex++) {
        gold += gEstatesGoldLevel
            [IDX(gpGame->m_heroRecs[m_players[player].m_heroIds[heroIndex]]
                     .m_secondarySkills[IDX(HERO_SKILL_ESTATES)])];
    }

    if (!gbHumanPlayer[player]) {
        if (gpGame->m_difficulty == DIFFICULTY_EASY)
            gold = static_cast<i32>(gold * GAME_DIFFICULTY_EASY_GOLD_FACTOR);
        if (gpGame->m_difficulty == DIFFICULTY_NORMAL) {
        }
        if (gpGame->m_difficulty == DIFFICULTY_HARD)
            gold = static_cast<i32>(gold * GAME_DIFFICULTY_HARD_GOLD_FACTOR);
        if (gpGame->m_difficulty == DIFFICULTY_EXPERT)
            gold = static_cast<i32>(gold * GAME_DIFFICULTY_EXPERT_GOLD_FACTOR);
        if (gpGame->m_difficulty == DIFFICULTY_IMPOSSIBLE)
            gold = static_cast<i32>(gold * GAME_DIFFICULTY_IMPOSSIBLE_GOLD_FACTOR);
    }

    if (m_playerHandicap[player] == PLAYER_HANDICAP_MODERATE)
        gold = static_cast<i32>(gold * GAME_HANDICAP_MODERATE_RESOURCE_FACTOR);
    else if (m_playerHandicap[player] == PLAYER_HANDICAP_SEVERE)
        gold = static_cast<i32>(gold * DATA_COMPGEN(0x004eb6f0, computeDailyGoldConstant, GAME_HANDICAP_SEVERE_RESOURCE_FACTOR));
    return gold;
}

VA(0x0047c761, 0x9aa)
void game::PerDay(void) {
    i32 maxSpellPoints9;
    i32 player;
    H2_ENUM_STORAGE_STEPPED(ResourceType, i32) resource8;
    i32 income13;
    i32 dailyIncome0;
    MineType resourceType1;
    hero* currentHero6;
    i32 restoredSpellPoints13;
    hero* townHero12;
    town* currentTown4;
    double penaltyRate9;

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource8 = RES_WOOD; resource8 < RES_COUNT; resource8++) {
            gpGame->m_players[player].m_aiData.m_income[IDX(resource8)] =
                -m_players[player].m_resources[IDX(resource8)];
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
                m_players[m_mines[player].owner].m_resources[IDX(resourceType1)] += dailyIncome0;
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
                m_players[player].m_resources[m_day - 1]++;
            if (gpGame->m_difficulty >= DIFFICULTY_IMPOSSIBLE
                && m_day >= DAILY_RESOURCE_BONUS_FIRST_DAY
                && m_day <= DAILY_RESOURCE_BONUS_LAST_DAY)
                m_players[player].m_resources[m_day - 1]++;
            if (gpGame->m_players[player].m_aiDifficulty == PLAYER_PERSONALITY_BUILDER
                && m_day >= DAILY_RESOURCE_BONUS_FIRST_DAY
                && m_day <= DAILY_RESOURCE_BONUS_LAST_DAY)
                m_players[player].m_resources[m_day - 1]++;
        }
    }

    m_day++;
    giCurTurn = (m_week - 1) * EVENT_DAYS_PER_WEEK + (m_month - 1) * EVENT_DAYS_PER_MONTH
                + m_day;
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
        for (resource8 = RES_WOOD; resource8 < RES_GOLD; resource8++) {
            if (m_playerHandicap[player] != PLAYER_HANDICAP_NONE) {
                if (m_playerHandicap[player] == PLAYER_HANDICAP_MODERATE)
                    penaltyRate9 = GAME_HANDICAP_MODERATE_DAILY_PENALTY;
                else
                    penaltyRate9 = GAME_HANDICAP_SEVERE_DAILY_PENALTY;
                m_players[player].m_resources[IDX(resource8)] -= static_cast<i32>(
                    (gpGame->m_players[player].m_aiData.m_income[IDX(resource8)]
                     + m_players[player].m_resources[IDX(resource8)])
                    * penaltyRate9
                );
            }
        }
    }

    for (player = 0; player < gpGame->m_playerCount; player++) {
        for (resource8 = RES_WOOD; resource8 < RES_COUNT; resource8++) {
            gpGame->m_players[player].m_aiData.m_income[IDX(resource8)] +=
                m_players[player].m_resources[IDX(resource8)];
        }
    }

    for (player = 0; player < GAME_HERO_COUNT; player++) {
        currentHero6 = &m_heroRecs[player];
        restoredSpellPoints13 = currentHero6->m_spellPoints;
        maxSpellPoints9 =
            currentHero6->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
        restoredSpellPoints13 +=
            IDX(currentHero6->m_secondarySkills[IDX(HERO_SKILL_MYSTICISM)]) + 1;
        if (currentHero6->HasArtifact(ARTIFACT_POWER_RING))
            restoredSpellPoints13 += POWER_RING_DAILY_MANA_BONUS;
        if (restoredSpellPoints13 > maxSpellPoints9)
            restoredSpellPoints13 = maxSpellPoints9;
        if (currentHero6->m_spellPoints < restoredSpellPoints13)
            currentHero6->m_spellPoints = static_cast<i16>(restoredSpellPoints13);
        if (HAS(currentHero6->m_eventFlags, HERO_EVENT_MAGIC_WELL))
            currentHero6->m_eventFlags = HeroEventFlag(
                static_cast<i32>(currentHero6->m_eventFlags) - IDX(HERO_EVENT_MAGIC_WELL)
            );
    }

    for (player = 0; player < GAME_TOWN_COUNT; player++) {
        currentTown4 = GetTown(player);
        if (!(currentTown4->m_buildings & BIT(BUILDING_SLOT_MAGE_GUILD)))
            continue;
        if (currentTown4->m_occupyingHeroId != -1) {
            townHero12 = GetHero(currentTown4->m_occupyingHeroId);
            maxSpellPoints9 =
                townHero12->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
            if (townHero12->m_spellPoints < maxSpellPoints9)
                townHero12->m_spellPoints = static_cast<i16>(maxSpellPoints9);
        }
    }
}

VA(0x0047d10b, 0x199d)
void game::PerWeek(void) {
    FactionType heroClass18 = FACTION_KNIGHT;
    i32 outerIndex5;
    i32 innerIndex3;
    i32 mapY5;
    i32 mapX8;
    town* castle37;
    i32 growth13;
    FactionType desiredClass1;
    i32 monsterIncrease16;
    i32 monsterCount36;
    hero* weeklyHero4;

    giWeekType = CALENDAR_PERIOD_NORMAL;
    giWeekTypeExtra = Random(0, WEEK_NAME_LAST);
    if (m_week != GAME_WEEKS_PER_MONTH) {
        outerIndex5 = Random(1, SPECIAL_WEEK_ROLL_MAX);
        if (outerIndex5 == 1) {
            giWeekType = CALENDAR_PERIOD_CREATURE;
            giWeekTypeExtra = Random(0, CREATURE_WEEK_LAST);
        }
    }

    for (outerIndex5 = 0; outerIndex5 < GAME_TOWN_COUNT; outerIndex5++) {
        castle37 = GetTown(outerIndex5);
        for (innerIndex3 = WEEKLY_FIRST_DWELLING; innerIndex3 <= WEEKLY_LAST_DWELLING;
             innerIndex3++) {
            if (castle37->m_buildings & (1 << innerIndex3)) {
                growth13 = gMonsterDatabase[IDX(gDwellingType[IDX(castle37->m_type)]
                                                             [innerIndex3 - WEEKLY_FIRST_DWELLING])]
                               .growth;
                if (castle37->m_buildings & BIT(BUILDING_SLOT_SPECIAL_FOUR))
                    growth13 += CASTLE_GROWTH_SPECIAL_BONUS;
                if (innerIndex3 == WEEKLY_FIRST_DWELLING
                    && (castle37->m_buildings & BIT(BUILDING_SLOT_WELL_EXTRA)))
                    growth13 += CASTLE_GROWTH_WELL_BONUS;
                if (castle37->m_owner == -1)
                    growth13 /= NEUTRAL_CASTLE_GROWTH_DIVISOR;
                if (castle37->m_owner >= 0
                    && castle37->m_garrison[innerIndex3 - WEEKLY_FIRST_DWELLING] == 0
                    && !gbHumanPlayer[castle37->m_owner]) {
                    if (gpGame->m_difficulty == DIFFICULTY_HARD)
                        growth13 = static_cast<i32>(growth13 * WEEKLY_HARD_GROWTH_FACTOR);
                    if (gpGame->m_difficulty == DIFFICULTY_EXPERT)
                        growth13 = static_cast<i32>(growth13 * WEEKLY_EXPERT_GROWTH_FACTOR);
                    if (gpGame->m_difficulty == DIFFICULTY_IMPOSSIBLE)
                        growth13 = static_cast<i32>(growth13 * WEEKLY_IMPOSSIBLE_GROWTH_FACTOR);
                }
                if (giWeekType == CALENDAR_PERIOD_CREATURE
                    && IDX(gDwellingType[IDX(castle37->m_type)]
                                        [innerIndex3 - WEEKLY_FIRST_DWELLING])
                           == giWeekTypeExtra)
                    growth13 += CREATURE_WEEK_GROWTH_BONUS;
                castle37->m_garrison[innerIndex3 - WEEKLY_FIRST_DWELLING] += growth13;
            }
        }
    }

    for (outerIndex5 = 0; outerIndex5 < GAME_PLAYER_COUNT; outerIndex5++) {
        for (innerIndex3 = 0; innerIndex3 < AVAILABLE_HERO_SLOTS; innerIndex3++) {
            if (innerIndex3 == 1) {
                heroClass18 =
                    m_heroRecs[gpGame->m_players[outerIndex5].m_availableHeroIds[0]].m_cursorType;
            }
            heroClass18 =
                (heroClass18 + Random(1, IDX(FACTION_COUNT) - 1)) % IDX(FACTION_COUNT);
            desiredClass1 = heroClass18;
            if (innerIndex3 == 0
                && m_setupPlayerRace[gcColorToSetupPos[m_players[outerIndex5].m_color]]
                       < FACTION_COUNT) {
                desiredClass1 =
                    m_setupPlayerRace[gcColorToSetupPos[m_players[outerIndex5].m_color]];
            }

            if (gpGame->m_availableHeroes[(
                    innerIndex3 - outerIndex5 + outerIndex5 * (sizeof(playerData) + 1)
                )[gpGame->m_players[0].m_availableHeroIds]]
                == WEEKLY_AVAILABLE_HERO) {
                if (HAS(gpGame
                            ->m_heroRecs[(
                                innerIndex3 - outerIndex5 + outerIndex5 * (sizeof(playerData) + 1)
                            )[gpGame->m_players[0].m_availableHeroIds]]
                            .m_eventFlags,
                        WEEKLY_HERO_RESERVED_FLAG))
                    continue;
            }
            {
                if (gpGame->m_availableHeroes[(
                        innerIndex3 - outerIndex5 + outerIndex5 * (sizeof(playerData) + 1)
                    )[gpGame->m_players[0].m_availableHeroIds]]
                    == WEEKLY_AVAILABLE_HERO)
                    gpGame->m_availableHeroes[(
                        innerIndex3 - outerIndex5 + outerIndex5 * (sizeof(playerData) + 1)
                    )[gpGame->m_players[0].m_availableHeroIds]] = -1;
                if (innerIndex3 == 1 && !gbHumanPlayer[outerIndex5])
                    desiredClass1 = FACTION_ANY;
                i32 useDifficultyBonus3 =
                    !gbHumanPlayer[outerIndex5] && gpGame->m_difficulty > DIFFICULTY_EASY;
                (
                    innerIndex3 - outerIndex5 + outerIndex5 * (sizeof(playerData) + 1)
                )[gpGame->m_players[0].m_availableHeroIds] =
                    static_cast<i8>(
                        gpGame->GetNewHeroId(outerIndex5, desiredClass1, useDifficultyBonus3)
                    );
                m_availableHeroes[(
                    innerIndex3 - outerIndex5 + outerIndex5 * (sizeof(playerData) + 1)
                )[gpGame->m_players[0].m_availableHeroIds]] = WEEKLY_AVAILABLE_HERO;
            }
        }
    }

    for (mapY5 = 0; MAP_HEIGHT > mapY5; mapY5++) {
        for (mapX8 = 0; mapX8 < MAP_WIDTH; mapX8++) {
            // Retail's per-site refresh values and growth ranges are encoded map-object
            // balance payload. The surrounding case labels provide their semantic owners.
            // NOLINTBEGIN(readability-magic-numbers)
            switch (WORLDMAP->Row(mapY5)[mapX8].m_triggerType) {
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER: {
                    monsterCount36 = WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                                     & IDX(MAP_MONSTER_COUNT_MASK);
                    monsterIncrease16 = monsterCount36 / EVENT_DAYS_PER_WEEK;
                    if (Random(1, EVENT_DAYS_PER_WEEK)
                        <= static_cast<i32>(monsterCount36 % EVENT_DAYS_PER_WEEK))
                        monsterIncrease16++;
                    monsterCount36 += monsterIncrease16;
                    if (monsterCount36 > WEEKLY_MONSTER_LIMIT)
                        monsterCount36 = WEEKLY_MONSTER_LIMIT;
                    WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata =
                        (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                         & IDX(MAP_MONSTER_GUARD_FLAG))
                        | monsterCount36;
                    break;
                }
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTESIAN_SPRING:
                    WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata = 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WATER_WHEEL:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                        != WEEKLY_WATER_WHEEL_EMPTY)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata = 2;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MAGIC_GARDEN:
                    WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata = Random(0, 1) ? 7 : 6;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WINDMILL:
                    WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata = Random(1, 5);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARCHER_HOUSE:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(2, 4);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_GOBLIN_HUT:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DWARF_COTTAGE:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(2, 4);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_PEASANT_HUT:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(5, 10);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_LOG_CABIN:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(5, 10);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DESERT_TENT:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WAGON_CAMP:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_HOUSE:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(4, 8);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SIRENS:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_WATCH_TOWER:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(1, 4);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RUINS:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TREE_CITY:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                        < WEEKLY_MONSTER_POPULATION_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(10, 20);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CAVE:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(3, 6);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXCAVATION:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(4, 8);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HALFLING_HOLE:
                    if (WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata < WEEKLY_GROWTH_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(5, 10);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_TROLL_BRIDGE:
                    if (!(WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                          & WEEKLY_DWELLING_NO_GROWTH_FLAG)
                        && WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                               < WEEKLY_DRAGON_CITY_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CITY_OF_DEAD:
                    if (!(WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                          & WEEKLY_DWELLING_NO_GROWTH_FLAG)
                        && WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                               < WEEKLY_DRAGON_CITY_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += Random(1, 3);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_DRAGON_CITY:
                    if (!(WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                          & WEEKLY_DWELLING_NO_GROWTH_FLAG)
                        && WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata
                               < WEEKLY_DRAGON_CITY_LIMIT)
                        WORLDMAP->GetCell(mapX8, mapY5)->m_objectMetadata += 1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXPANSION_DWELLING:
                    WeeklyRecruitSite(WORLDMAP->GetCell(mapX8, mapY5));
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EXPANSION_OBJECT:
                    WeeklyGenericSite(WORLDMAP->GetCell(mapX8, mapY5));
                    break;
                default:
                    break;
            }
            // NOLINTEND(readability-magic-numbers)
        }
    }

    for (outerIndex5 = 0; outerIndex5 < GAME_HERO_COUNT; outerIndex5++) {
        weeklyHero4 = &m_heroRecs[outerIndex5];
        if (HAS(weeklyHero4->m_eventFlags, WEEKLY_HERO_VISIT_FLAG))
            weeklyHero4->m_eventFlags = weeklyHero4->m_eventFlags - WEEKLY_HERO_VISIT_FLAG;
    }

    m_week++;
    SetupNewRumour();
    GiveTroopsToNeutralTowns();
}

VA(0x0047eaa8, 0x12d)
void game::WeeklyRecruitSite(mapCell* cell) {
    i32 type = cell->m_objectMetadata;
    type &= WEEKLY_RECRUIT_TYPE_MASK;
    i32 recruitCount = cell->m_objectMetadata;
    recruitCount >>= WEEKLY_RECRUIT_COUNT_SHIFT;
    i32 packed;

    // The low metadata bits are serialized expansion-site subtype ids. Retail
    // defines five subtypes here, all with the same weekly growth rule.
    // NOLINTBEGIN(readability-magic-numbers)
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
    // NOLINTEND(readability-magic-numbers)

    if (recruitCount > WEEKLY_RECRUIT_LIMIT)
        recruitCount = WEEKLY_RECRUIT_LIMIT;
    packed = (recruitCount << WEEKLY_RECRUIT_COUNT_SHIFT) | type;
    cell->m_objectMetadata = packed | 0;
}

VA(0x0047ebd5, 0x6f)
void game::WeeklyGenericSite(mapCell* cell) {
    i32 type = cell->m_objectMetadata;
    type &= WEEKLY_SITE_TYPE_MASK;
    // Expansion-object subtype 4 is the only generic site with weekly state.
    // NOLINTBEGIN(readability-magic-numbers)
    switch (type) {
        case 4:
            cell->m_objectMetadata = type;
            break;
    }
    // NOLINTEND(readability-magic-numbers)
}

VA(0x0047ec44, 0x375)
void game::PerMonth(void) {
    mapCell* cell0;
    i32 mapX8;
    i32 mapY5;
    i32 townIndex0;
    i32 building4;
    i32 growth9;
    town* castle10;
    i32 firstCount5;
    i32 secondCount4;

    m_month++;
    townIndex0 = Random(ROLL_MIN, ROLL_MAX);
    if (townIndex0 <= NORMAL_ROLL_MAX) {
        giMonthType = CALENDAR_PERIOD_NORMAL;
        giMonthTypeExtra = Random(NORMAL_NAME_MIN, NORMAL_NAME_MAX);
    } else if (townIndex0 <= CREATURE_ROLL_MAX) {
        giMonthType = CALENDAR_PERIOD_CREATURE;
        giMonthTypeExtra = giMonType[Random(CREATURE_LIST_MIN, CREATURE_LIST_MAX)];
    } else {
        giMonthType = CALENDAR_PERIOD_PLAGUE;
    }

    for (townIndex0 = 0; townIndex0 < GAME_TOWN_COUNT; townIndex0++) {
        for (building4 = WEEKLY_FIRST_DWELLING; building4 <= WEEKLY_LAST_DWELLING; building4++) {
            castle10 = GetTown(townIndex0);
            if (castle10->m_buildings & (1 << building4)) {
                growth9 = gMonsterDatabase[IDX(gDwellingType[IDX(castle10->m_type)]
                                                            [building4 - WEEKLY_FIRST_DWELLING])]
                              .growth;
                if (castle10->m_buildings & WELL_BUILDING)
                    growth9 += WELL_GROWTH;
                if (building4 == WEEKLY_FIRST_DWELLING
                    && (castle10->m_buildings & FIRST_DWELLING_BONUS_BUILDING))
                    growth9 += FIRST_DWELLING_GROWTH;

                if (giMonthType == CALENDAR_PERIOD_CREATURE
                    && IDX(gDwellingType[IDX(castle10->m_type)]
                                        [building4 - WEEKLY_FIRST_DWELLING])
                           == giMonthTypeExtra)
                    castle10->m_garrison[building4 - WEEKLY_FIRST_DWELLING] *=
                        CREATURE_MONTH_MULTIPLIER;

                if (giMonthType == CALENDAR_PERIOD_PLAGUE) {
                    castle10->m_garrison[building4 - WEEKLY_FIRST_DWELLING] -= growth9;
                    if (castle10->m_garrison[building4 - WEEKLY_FIRST_DWELLING] < 0)
                        castle10->m_garrison[building4 - WEEKLY_FIRST_DWELLING] = 0;
                    castle10->m_garrison[building4 - WEEKLY_FIRST_DWELLING] =
                        castle10->m_garrison[building4 - WEEKLY_FIRST_DWELLING] >> 1;
                }
            }
        }
    }

    if (giMonthType == CALENDAR_PERIOD_CREATURE) {
        for (mapX8 = 0; mapX8 < MAP_WIDTH; mapX8++) {
            for (mapY5 = 0; mapY5 < MAP_HEIGHT; mapY5++) {
                cell0 = gpAdvManager->GetCell(mapX8, mapY5);
                if (cell0->m_triggerType == MAP_OBJECT_NONE && !cell0->m_objectLayerBit1
                    && !cell0->m_objectLayerBit0
                    && giGroundToTerrain[cell0->m_terrainImageIndex] != TERRAIN_WATER) {
                    if (Random(MONSTER_SPAWN_MIN, MONSTER_SPAWN_MAX)
                        == MONSTER_SPAWN_ROLL) {
                        cell0->m_triggerType = MONSTER_TRIGGER;
                        cell0->m_objectTileset = TILESET_MONS32;
                        cell0->m_objectIndex = static_cast<u8>(giMonthTypeExtra);
                        firstCount5 = GetRandomNumTroops(
                            static_cast<CreatureType>(giMonthTypeExtra)
                        );
                        secondCount4 = GetRandomNumTroops(
                            static_cast<CreatureType>(giMonthTypeExtra)
                        );
                        cell0->m_objectMetadata = (firstCount5 + secondCount4) | 0;
                        if (Random(MONSTER_GUARD_ROLL_MIN, MONSTER_GUARD_ROLL_MAX)
                            < MONSTER_GUARD_CUTOFF)
                            cell0->m_objectMetadata |= IDX(MAP_MONSTER_GUARD_FLAG);
                    }
                }
            }
        }
    }

    gpAdvManager->CompleteDraw(0);
}

VA(0x0047efb9, 0x476)
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

    for (x = left; right >= x; x++) {
        for (y = top; bottom >= y; y++) {
            if (x < 0 || x >= MAP_WIDTH || y < 0 || MAP_HEIGHT <= y)
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

VA(0x0047f42f, 0x1c2)
void game::RandomizeTown(i32 x, i32 y, i32) {
    i32 unused6[RANDOM_TOWN_SCRATCH_WIDTH];
    i32 townId0 = GetTownId(x, y);
    town* castle0 = GetTown(townId0);
    mapTownExtra* extra =
        reinterpret_cast<mapTownExtra*>(ppMapExtra[WORLDMAP->GetCell(x, y)->m_objectMetadata]);
    FactionType race0;

    if (extra->color == RANDOM_TOWN_UNOWNED_COLOR)
        race0 = static_cast<FactionType>(Random(RANDOM_TOWN_RACE_MIN, RANDOM_TOWN_RACE_MAX));
    else
        race0 = m_setupPlayerRace[gcColorToSetupPos[extra->color]];

    castle0->m_turnsOwned = RANDOM_TOWN_AGE;
    ConvertObject(
        x + RANDOM_TOWN_LEFT,
        y + RANDOM_TOWN_TOP,
        x + RANDOM_TOWN_RIGHT,
        y + RANDOM_TOWN_BOTTOM,
        RANDOM_TOWN_SOURCE_TILESET,
        RANDOM_TOWN_OBJECT_SOURCE_FIRST,
        RANDOM_TOWN_OBJECT_SOURCE_LAST,
        RANDOM_TOWN_OBJECT_TILESET,
        IDX(race0) << RANDOM_TOWN_RACE_FRAME_SHIFT,
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
        IDX(race0) << RANDOM_TOWN_RACE_FRAME_SHIFT,
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
        IDX(race0) << RANDOM_TOWN_RACE_FRAME_SHIFT,
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
        IDX(race0) << RANDOM_TOWN_RACE_FRAME_SHIFT,
        MAP_OBJECT_RANDOM_CASTLE,
        MAP_OBJECT_CASTLE
    );
    m_castleRecs[townId0].m_type = race0;
}

VA(0x0047f5f1, 0x619)
void game::RandomizeMine(i32 x, i32 y) {
    u8 objectFrame1;
    i32 mineId;
    MineType mineType29;
    H2_ENUM_STORAGE(TerrainType, i32) terrain3;
    i32 columnOffset4;
    i32 retry4;
    i32 rowOffset0;
    u8 mineFrame36;
    MapObjectType triggerType19;

    terrain3 = giGroundToTerrain[WORLDMAP->GetCell(x, y)->m_terrainImageIndex];
    for (retry4 = 0; retry4 < RANDOM_MINE_RETRY_LIMIT; retry4++) {
        switch (terrain3) {
            case TERRAIN_GRASS:
            case TERRAIN_DIRT:
                mineType29 = static_cast<MineType>(Random(IDX(MINE_TYPE_MERCURY), IDX(MINE_TYPE_GOLD)));
                if (mineType29 == MINE_TYPE_MERCURY)
                    mineType29 = MINE_TYPE_WOOD;
                break;
            case TERRAIN_SNOW:
                mineType29 = static_cast<MineType>(Random(IDX(MINE_TYPE_ORE), IDX(MINE_TYPE_GOLD)));
                break;
            case TERRAIN_SWAMP:
                mineType29 = static_cast<MineType>(Random(IDX(MINE_TYPE_WOOD), IDX(MINE_TYPE_GOLD)));
                break;
            case TERRAIN_LAVA:
                mineType29 = MINE_TYPE_MERCURY;
                break;
            default:
                mineType29 = static_cast<MineType>(Random(IDX(MINE_TYPE_MERCURY), IDX(MINE_TYPE_GOLD)));
                break;
        }
        if (RandMineQty[IDX(mineType29)] == 0)
            retry4 = RANDOM_MINE_RETRY_LIMIT;
    }
    RandMineQty[IDX(mineType29)]++;

    // Mine overlays and object bodies use terrain-specific sprite-frame
    // payload from the original map tilesets.
    // NOLINTBEGIN(readability-magic-numbers)
    switch (mineType29) {
        case MINE_TYPE_WOOD:
            mineFrame36 = 5;
            break;
        case MINE_TYPE_MERCURY:
            mineFrame36 = 25;
            break;
        default:
            switch (terrain3) {
                case TERRAIN_GRASS:
                    mineFrame36 = 15;
                    break;
                case TERRAIN_SNOW:
                    mineFrame36 = 19;
                    break;
                default:
                    mineFrame36 = 9;
                    break;
            }
            break;
    }

    switch (mineType29) {
        case MINE_TYPE_WOOD:
            objectFrame1 = 7;
            break;
        case MINE_TYPE_MERCURY:
            switch (terrain3) {
                case TERRAIN_SWAMP:
                    objectFrame1 = 43;
                    break;
                case TERRAIN_LAVA:
                    objectFrame1 = 35;
                    break;
                default:
                    objectFrame1 = 27;
                    break;
            }
            break;
        default:
            switch (terrain3) {
                case TERRAIN_GRASS:
                    objectFrame1 = 17;
                    break;
                case TERRAIN_SNOW:
                    objectFrame1 = 21;
                    break;
                case TERRAIN_SWAMP:
                    objectFrame1 = 23;
                    break;
                case TERRAIN_DESERT:
                    objectFrame1 = 13;
                    break;
                default:
                    objectFrame1 = 11;
                    break;
            }
            break;
    }
    // NOLINTEND(readability-magic-numbers)

    WORLDMAP->GetCell(x, y)->m_objectIndex = objectFrame1;
    WORLDMAP->GetCell(x + 1, y)->m_objectIndex = objectFrame1 + 1;
    WORLDMAP->GetCell(x, y - 1)->m_overlayIndex = mineFrame36;
    WORLDMAP->GetCell(x + 1, y - 1)->m_overlayIndex = mineFrame36 + 1;

    if (mineType29 == MINE_TYPE_MERCURY) {
        WORLDMAP->GetCell(x + 1, y)->m_objType |= 1;
        triggerType19 = MAP_OBJECT_ALCHEMIST_LAB;
    } else if (mineType29 == MINE_TYPE_WOOD) {
        triggerType19 = MAP_OBJECT_SAWMILL;
    } else {
        m_worldMap.ChangeTilesetIndex(
            WORLDMAP->GetCell(x + 1, y),
            x + 1,
            y,
            TILESET_EXTRAOVR,
            IDX(mineType29) - IDX(MINE_TYPE_ORE),
            0,
            -1
        );
        triggerType19 = MAP_OBJECT_MINE;
    }

    mineId = GetMineId(x, y);
    for (rowOffset0 = 0; rowOffset0 < RANDOM_MINE_FOOTPRINT_WIDTH; rowOffset0++) {
        for (columnOffset4 = 0; columnOffset4 < RANDOM_MINE_FOOTPRINT_WIDTH; columnOffset4++) {
            if ((WORLDMAP->GetCell(columnOffset4 + x, y - rowOffset0)->m_triggerType
                 & MAP_TRIGGER_TYPE_MASK)
                > MAP_OBJECT_NONE)
                if (IDX(
                        WORLDMAP->GetCell(columnOffset4 + x, y - rowOffset0)->m_triggerType
                        & MAP_TRIGGER_TYPE_MASK
                    )
                    <= MINE_FLAG_OVERWRITE_LIMIT)
                    continue;
            WORLDMAP->GetCell(columnOffset4 + x, y - rowOffset0)->m_objectMetadata = mineId;
            WORLDMAP->GetCell(columnOffset4 + x, y - rowOffset0)->m_triggerType = triggerType19;
        }
    }
    WORLDMAP->GetCell(x, y)->m_triggerType |= MAP_TRIGGER_ACTION_FLAG;
    m_mines[mineId].resourceType = mineType29;
}

VA(0x0047fc0a, 0xc6)
void game::InitRandomArtifacts(void) {
    // Retail /Od retains this otherwise unused local between the loop-index slots.
    i32 ignoredIndex;
    i32 y;
    memset(m_randomArtifacts, 0, sizeof(m_randomArtifacts));
    for (i32 x = 0; MAP_WIDTH > x; x++) {
        for (y = 0; y < MAP_HEIGHT; y++) {
            mapCell* cell = WORLDMAP->Row(y) + x;
            if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT))
                m_randomArtifacts[cell->m_objectIndex >> 1] = 1;
        }
    }
}

VA(0x0047fcd0, 0x17f)
i32 game::GetRandomArtifactId(
    H2_ENUM_PARAM(ArtifactLevelMask, i32) levelMask,
    b32 allowCursed
) {
    i32 attempts = 0;
    ArtifactType artifact;

    while (1) {
        if (xIsExpansionMap)
            artifact = static_cast<ArtifactType>(Random(ARTIFACT_FIRST, ARTIFACT_EXPANSION_LAST));
        else
            artifact = static_cast<ArtifactType>(Random(ARTIFACT_FIRST, ARTIFACT_BASE_LAST));

        if (!HAS(levelMask, gArtifactLevel[IDX(artifact)]))
            continue;
        if (artifact == ARTIFACT_EDITOR_ANY_ULTIMATE || artifact == ARTIFACT_EDITOR_UNUSED_84
            || artifact == ARTIFACT_EDITOR_UNUSED_85 || artifact == ARTIFACT_EDITOR_UNUSED_86
            || artifact == ARTIFACT_SPELL_SCROLL || artifact == ARTIFACT_BREASTPLATE_ANDURAN
            || artifact == ARTIFACT_BATTLE_GARB || artifact == ARTIFACT_HELMET_ANDURAN
            || artifact == ARTIFACT_SWORD_ANDURAN || artifact == ARTIFACT_SPHERE_NEGATION)
            continue;
        if (attempts++ < ARTIFACT_UNIQUE_RETRIES && m_randomArtifacts[IDX(artifact)])
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

VA(0x0047fe4f, 0x68)
i32 IsCursedItem(ArtifactType item) {
    if (item == ARTIFACT_FIZBIN_OF_MISFORTUNE || item == ARTIFACT_HIDEOUS_MASK
        || item == ARTIFACT_TAX_LIEN || item == ARTIFACT_ARM_OF_MARTYR
        || item == ARTIFACT_BROACH_SHIELDING || item == ARTIFACT_HEART_FIRE
        || item == ARTIFACT_HEART_ICE)
        return 1;
    return 0;
}

VA(0x0047feb7, 0x1ef)
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

VA(0x004800a6, 0x378)
void game::SetRandomHeroArmies(i32 heroId, i32 strongArmy) {
    armyGroup* army2 = &m_heroRecs[heroId].m_army;
    i32 armySlot7 = 0;
    // Starting-army count ranges are retail faction-balance payload.
    // NOLINTBEGIN(readability-magic-numbers)
    RandomHeroArmyRange armyTable7[IDX(FACTION_COUNT)][RANDOM_HERO_ARMY_OPTION_COUNT] = {
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
    // NOLINTEND(readability-magic-numbers)
    i32 selected9[RANDOM_HERO_ARMY_OPTION_COUNT];
    i32 index9;
    i32 minimum5;
    i32 maximum5;

    selected9[0] = RANDOM_HERO_STACK_SELECTED;
    selected9[1] =
        RANDOM_HERO_FIRST_STACK_CHANCE + (strongArmy ? RANDOM_HERO_FIRST_STACK_BONUS_CHANCE : 0)
        > Random(RANDOM_HERO_PERCENT_MIN, RANDOM_HERO_PERCENT_MAX);
    selected9[RANDOM_HERO_SECOND_SELECTION] =
        Random(RANDOM_HERO_PERCENT_MIN, RANDOM_HERO_PERCENT_MAX)
        < RANDOM_HERO_SECOND_STACK_CHANCE
              + (strongArmy ? RANDOM_HERO_SECOND_STACK_BONUS_CHANCE : 0);
    if (!selected9[RANDOM_HERO_SECOND_SELECTION])
        selected9[1] = RANDOM_HERO_STACK_SELECTED;

    for (index9 = 0; index9 < RANDOM_HERO_ARMY_SLOT_COUNT; index9++) {
        army2->m_creatureTypes[index9] = CREATURE_NONE;
        army2->m_creatureCounts[index9] = RANDOM_HERO_EMPTY_COUNT;
    }

    for (index9 = 0; index9 < RANDOM_HERO_ARMY_SELECTION_COUNT; index9++) {
        if (selected9[index9]) {
            army2->m_creatureTypes[armySlot7] =
                static_cast<i8>(armyTable7[IDX(m_heroRecs[heroId].m_cursorType)][index9].creature);
            minimum5 = armyTable7[IDX(m_heroRecs[heroId].m_cursorType)][index9].minimum
                       * RANDOM_HERO_COUNT_SCALE;
            maximum5 = armyTable7[IDX(m_heroRecs[heroId].m_cursorType)][index9].maximum
                           * RANDOM_HERO_COUNT_SCALE
                       + RANDOM_HERO_COUNT_ROUNDING;
            if (strongArmy)
                minimum5 =
                    (minimum5 + maximum5) / RANDOM_HERO_AVERAGE_DIVISOR;
            army2->m_creatureCounts[armySlot7] =
                static_cast<i16>(Random(minimum5, maximum5) / RANDOM_HERO_COUNT_SCALE);
            armySlot7++;
        }
    }
}

VA(0x0048041e, 0x746)
void game::ProcessRandomObjects(void) {
    i32 maxValue17;
    i32 x10;
    i32 mineIndex8;
    i32 y8;
    i32 artifactId18;
    i32 minValue7;
    mapCell* cell6;
    i32 randomType0;
    MapObjectType randomObjectType3;

    giUABaseX = -1;
    giUABaseY = -1;
    giUARadius = 0;
    for (mineIndex8 = 0; mineIndex8 < RANDOM_MINE_RESOURCE_COUNT; mineIndex8++)
        RandMineQty[mineIndex8] = 0;

    for (y8 = 0; MAP_HEIGHT > y8; y8++) {
        for (x10 = 0; x10 < MAP_WIDTH; x10++) {
            cell6 = WORLDMAP->GetCell(x10, y8);
            switch (cell6->m_triggerType) {
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT:
                    giUABaseX = static_cast<i16>(x10);
                    giUABaseY = static_cast<i16>(y8);
                    giUARadius = static_cast<i16>(cell6->m_objectMetadata);
                    cell6->m_triggerType = MAP_OBJECT_NONE;
                    cell6->m_objectTileset = TILESET_NONE;
                    cell6->m_objectIndex = -1;
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_TOWN:
                    RandomizeTown(x10, y8, 0);
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_CASTLE:
                    RandomizeTown(x10, y8, 1);
                    break;
                // Random-object value bands, quantity ranges, and paired source
                // frames are serialized map-editor balance and tileset payload.
                // NOLINTBEGIN(readability-magic-numbers)
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER:
                    minValue7 = 80;
                    maxValue17 = 2000;
                    goto randomMonster;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_WEAK:
                    minValue7 = 0;
                    maxValue17 = 400;
                    goto randomMonster;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_MEDIUM:
                    minValue7 = 400;
                    maxValue17 = 1000;
                    goto randomMonster;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_STRONG:
                    minValue7 = 1000;
                    maxValue17 = 2500;
                    goto randomMonster;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG:
                    minValue7 = 2500;
                    maxValue17 = 100000;
                    goto randomMonster;
                randomMonster:
                    if (cell6->m_objectTileset == TILESET_MONS32
                        && cell6->m_objectIndex >= RANDOM_MONSTER_SPRITE_FIRST
                        && cell6->m_objectIndex <= RANDOM_MONSTER_SPRITE_LAST) {
                        randomObjectType3 = static_cast<MapObjectType>(
                            cell6->m_objectIndex + RANDOM_MONSTER_SPRITE_TO_TRIGGER
                        );
                        switch (randomObjectType3) {
                            case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_WEAK:
                                minValue7 = 0;
                                maxValue17 = 400;
                                goto monsterBoundsReady;
                            case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_MEDIUM:
                                minValue7 = 400;
                                maxValue17 = 1000;
                                goto monsterBoundsReady;
                            case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_STRONG:
                                minValue7 = 1000;
                                maxValue17 = 2500;
                                goto monsterBoundsReady;
                            case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG:
                                minValue7 = 2500;
                                maxValue17 = 100000;
                                goto monsterBoundsReady;
                        }
                    }
                monsterBoundsReady:
                    cell6->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER;
                    cell6->m_objectIndex = static_cast<u8>(Random(0, 65));
                    while (gMonsterDatabase[cell6->m_objectIndex].randomValue <= minValue7
                           || gMonsterDatabase[cell6->m_objectIndex].randomValue >= maxValue17)
                        cell6->m_objectIndex = static_cast<u8>(Random(0, 65));
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_RESOURCE:
                    cell6->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RESOURCE;
                    randomType0 = Random(0, 6);
                    ConvertObject(
                        x10 - 1,
                        y8,
                        x10 - 1,
                        y8,
                        TILESET_OBJNRSRC,
                        16,
                        16,
                        TILESET_OBJNRSRC,
                        randomType0 * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x10,
                        y8,
                        x10,
                        y8,
                        TILESET_OBJNRSRC,
                        17,
                        17,
                        TILESET_OBJNRSRC,
                        randomType0 * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    switch (randomType0) {
                        case 0:
                        case 2:
                            cell6->m_objectMetadata = Random(8, 16) | 0;
                            break;
                        case 6:
                            cell6->m_objectMetadata = Random(5, 10) | 0;
                            break;
                        default:
                            cell6->m_objectMetadata = Random(3, 7) | 0;
                            break;
                    }
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_ARTIFACT:
                    artifactId18 = GetRandomArtifactId(ARTIFACT_LEVEL_RANDOM, false);
                    cell6->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT;
                    ConvertObject(
                        x10 - 1,
                        y8,
                        x10 - 1,
                        y8,
                        TILESET_OBJNARTI,
                        162,
                        162,
                        TILESET_OBJNARTI,
                        artifactId18 * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x10,
                        y8,
                        x10,
                        y8,
                        TILESET_OBJNARTI,
                        163,
                        163,
                        TILESET_OBJNARTI,
                        artifactId18 * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_TREASURE_ARTIFACT:
                    artifactId18 = GetRandomArtifactId(ARTIFACT_LEVEL_TREASURE, false);
                    cell6->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT;
                    ConvertObject(
                        x10 - 1,
                        y8,
                        x10 - 1,
                        y8,
                        TILESET_OBJNARTI,
                        166,
                        166,
                        TILESET_OBJNARTI,
                        artifactId18 * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x10,
                        y8,
                        x10,
                        y8,
                        TILESET_OBJNARTI,
                        167,
                        167,
                        TILESET_OBJNARTI,
                        artifactId18 * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MINOR_ARTIFACT:
                    artifactId18 = GetRandomArtifactId(ARTIFACT_LEVEL_MINOR, false);
                    cell6->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT;
                    ConvertObject(
                        x10 - 1,
                        y8,
                        x10 - 1,
                        y8,
                        TILESET_OBJNARTI,
                        168,
                        168,
                        TILESET_OBJNARTI,
                        artifactId18 * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x10,
                        y8,
                        x10,
                        y8,
                        TILESET_OBJNARTI,
                        169,
                        169,
                        TILESET_OBJNARTI,
                        artifactId18 * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    break;
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RANDOM_MAJOR_ARTIFACT:
                    artifactId18 = GetRandomArtifactId(ARTIFACT_LEVEL_MAJOR, false);
                    cell6->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT;
                    ConvertObject(
                        x10 - 1,
                        y8,
                        x10 - 1,
                        y8,
                        TILESET_OBJNARTI,
                        170,
                        170,
                        TILESET_OBJNARTI,
                        artifactId18 * 2,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    ConvertObject(
                        x10,
                        y8,
                        x10,
                        y8,
                        TILESET_OBJNARTI,
                        171,
                        171,
                        TILESET_OBJNARTI,
                        artifactId18 * 2 + 1,
                        MAP_OBJECT_NO_CONVERSION,
                        MAP_OBJECT_NO_CONVERSION
                    );
                    break;
                // NOLINTEND(readability-magic-numbers)
                // The legacy random-mine trigger reuses the flagged value whose low
                // seven bits later identify an Eye of the Magi map object.
                case MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_EYE_OF_MAGI:
                    RandomizeMine(x10, y8);
                    break;
            }
        }
    }
}

VA(0x00480b64, 0x230)
void game::SetVisibility(i32 x, i32 y, i32 player, i32 radius) {
    i32 col;
    i32 cutoff;
    i32 row;
    u8 mask = static_cast<u8>(1 << player);
    i32 visibility;

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
        for (row = 0; row < MAP_HEIGHT; row++) {
            for (col = 0; col < MAP_WIDTH; col++) {
                i32 distance = static_cast<i32>(
                    sqrt(static_cast<double>((x - col) * (x - col) + (y - row) * (y - row)))
                );
                if (distance < radius) {
                    mapExtra[MAP_WIDTH * row + col] |= mask;
                }
            }
        }
    } else {
        for (row = y - radius; row <= y + radius; row++) {
            for (col = x - radius; col <= x + radius; col++) {
                visibility = radius - abs(y - row) + radius - abs(x - col);
                if (visibility >= cutoff && col >= 0 && row >= 0 && col < MAP_WIDTH
                    && row < MAP_HEIGHT) {
                    mapExtra[MAP_WIDTH * row + col] |= mask;
                }
            }
        }
    }
}

VA(0x00480d94, 0xd8)
void game::MakeAllWaterVisible(i32 player) {
    char mask = static_cast<char>(1 << player);
    i32 x;
    i32 y;
    for (x = 0; x < MAP_WIDTH; x++) {
        for (y = 0; y < MAP_HEIGHT; y++) {
            if (giGroundToTerrain[WORLDMAP->Row(y)[x].m_terrainImageIndex] == TERRAIN_WATER)
                mapExtra[y * MAP_WIDTH + x] |= mask;
        }
    }
}

VA(0x00480e6c, 0xfc)
void game::GiveArmy(
    armyGroup* group,
    H2_ENUM_PARAM(CreatureType, i32) type,
    i32 count,
    i32 slot
) {
    i32 tmp;
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

VA(0x00480f68, 0x91)
i32 game::ExperienceValueOfStack(armyGroup* group, hero* h) {
    i32 exp = 0;
    i32 i;
    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        if (group->m_quantities[i] > 0) {
            exp += gMonsterDatabase[IDX(group->m_creatureTypes[i])].hitPoints
                * group->m_quantities[i];
        }
    }
    if (h != NULL)
        exp += EXPERIENCE_HERO_PRESENCE_BONUS;
    return exp;
}

VA(0x00480ff9, 0x126)
i32 game::GetLuck(hero* h, class army*, town* castle) {
    i32 luck;
    if (h == NULL)
        return NEUTRAL;
    luck = NEUTRAL;
    if (h->HasArtifact(ARTIFACT_RABBIT_FOOT))
        luck++;
    if (h->HasArtifact(ARTIFACT_GOLDEN_HORSESHOE))
        luck++;
    if (h->HasArtifact(ARTIFACT_GAMBLERS_COIN))
        luck++;
    if (h->HasArtifact(ARTIFACT_FOUR_LEAF_CLOVER))
        luck++;
    if (h->HasArtifact(ARTIFACT_MASTHEAD) && HAS(h->m_eventFlags, HERO_EVENT_EMBARKED)) {
        luck++;
    }
    luck += h->m_luck;
    luck += IDX(h->m_secondarySkills[IDX(HERO_SKILL_LUCK)]);
    if (luck < MINIMUM)
        luck = MINIMUM;
    if (luck > MAXIMUM)
        luck = MAXIMUM;
    if (h->HasArtifact(ARTIFACT_BATTLE_GARB))
        luck = MAXIMUM;
    if (castle != NULL && castle->m_type == FACTION_SORCERESS
        && (castle->m_buildings & IDX(TOWN_BUILDING_RAINBOW))) {
        luck += RAINBOW_BONUS;
    }
    return luck;
}

VA(0x0048111f, 0xf1)
void game::SetupAdjacentMons(void) {
    i32 col;
    i32 row;
    u8 mask = IDX(MAP_EXTRA_ADJACENT_CLEAR_MASK);
    {
        i32 x;
        i32 y;
        for (x = 0; x < MAP_WIDTH; x++) {
            for (y = 0; y < MAP_HEIGHT; y++) {
                if (gpAdvManager->FindAdjacentMonster(x, y, &col, &row, -1, -1))
                    mapExtra[y * MAP_WIDTH + x] |= IDX(MAP_EXTRA_ADJACENT_MONSTER);
                else
                    mapExtra[y * MAP_WIDTH + x] &= IDX(mask);
            }
        }
    }
}

VA(0x00481210, 0x61)
void game::CancelComputerScreen(void) {
    TurnOffAIMusic();
    bShowIt = 1;
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

VA(0x00481271, 0xed)
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
                IDX(WIDGET_FLAG_UPDATE | WIDGET_FLAG_DIMMED)
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

VA(0x0048135e, 0xa0)
void game::ShowHeroesLogo(void) {
    if (gpAdvManager->m_openState == 0) {
        gpAdvManager->m_openState = 1;
        icon* theIcon = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f755c, theIconHerologoIcn, "herologo.icn"));
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

VA(0x004813fe, 0x143)
void game::WaitForPlayer(char* text, i32 player) {
    if (gbBlackoutPlayer && giNumHumanPlayers > 1 && !gbRemoteOn) {
        gpMouseManager->SetPointer(0);
        gbAllBlack = true;
        giBottomViewOverrideEndTime = KBTickCount() + WAIT_BOTTOM_VIEW_TIMEOUT;
        giBottomViewOverride = gbThisNetHumanPlayer[giCurPlayer] ? BOTTOM_VIEW_NEW_TURN
                                                                 : BOTTOM_VIEW_NONE;
        gpSoundManager->m_samplesReady = 1;
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

// Retail /Ob1 includes an inline-accessor continuation in this function.
VA(0x00481541, 0x104)
i32 game::HasLateOverlay(i32 col, i32 row) {
    mapCell* cell = WORLDMAP->Row(row) + col;
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

// Retail /Ob1 includes an inline-accessor continuation in this function.
VA(0x00481645, 0x120)
void game::ConvertFlagToLateOverlay(i32 col, i32 row) {
    mapCell* cell = WORLDMAP->Row(row) + col;
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

// Retail /Ob1 includes an inline-accessor continuation in this function.
VA(0x00481765, 0x13b)
i32 game::HasObjectTilesetIndex(
    i32 col,
    i32 row,
    H2_ENUM_PARAM(TilesetId, i32) tileset,
    i32 index
) {
    mapCell* cell = WORLDMAP->Row(row) + col;
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

// Retail /Ob1 includes an inline-accessor continuation in this function.
VA(0x004818a0, 0x112)
void game::ConvertAllToLateOverlay(i32 col, i32 row) {
    mapCell* cell = WORLDMAP->Row(row) + col;
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

VA(0x004819b2, 0x295)
void game::ProcessMapExtra(void) {
    i32 unused;
    i32 cost;
    i32 row16;
    mapCell* cell10;
    i32 col6;
    i32 townId;

    for (row16 = 0; row16 < MAP_HEIGHT; row16++) {
        for (col6 = 0; MAP_WIDTH > col6; col6++) {
            cell10 = WORLDMAP->Row(row16) + col6;
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
        for (col6 = 0; MAP_WIDTH > col6; col6++) {
            cell10 = WORLDMAP->Row(row16) + col6;
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
        for (col6 = 0; MAP_WIDTH > col6; col6++) {
            if (HasLateOverlay(col6, row16))
                ConvertAllToLateOverlay(col6, row16);
        }
    }
}

VA(0x00481c47, 0x900)
void game::SetupTowns(void) {
    DATA(0x004f756c) static i16 setupTownsSourceLineBase = 0x17f9;
    char defaultDwellingRoll[DEFAULT_DWELLING_ROLL_CAPACITY];
    i8 usedSpells[IDX(SPELL_COUNT)];
    i32 spellsPerLevel[TOWN_MAGE_GUILD_LEVEL_COUNT];
    i32 townIndex;
    i32 slot;
    i32 owner;
    i32 building;
    i32 spellLevel;
    i32 spellSlot;
    SpellType spell;
    i32 roll;
    i32 attempts;
    i32 spellValue;
    i32 combatSpells;
    i32 dwellingCount;
    u32 extraIndex;
    town* castle;
    mapTownExtra* extra;

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
                (gTownEligibleBuildMask[IDX(castle->m_type)] & extra->buildings)
                | (castle->m_buildings & (IDX(TOWN_BUILDING_CASTLE) | IDX(TOWN_BUILDING_TENT)));
            castle->m_buildState = extra->mageGuildLevel;
        } else {
            // The ten-entry table and AI adjustment are retail town-generation
            // probability payload.
            // NOLINTBEGIN(readability-magic-numbers)
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
            if (!gbHumanPlayer[castle->m_owner] && dwellingCount == 1 && Random(1, 10) < 4)
                dwellingCount++;
            // NOLINTEND(readability-magic-numbers)
            if (--dwellingCount != 0)
                castle->m_buildings |= IDX(TOWN_BUILDING_DWELLING_2);
            dwellingCount--;
            castle->m_buildState = 0;
        }

        for (building = TOWN_UPGRADE_BUILDING_FIRST; building <= TOWN_UPGRADE_BUILDING_LAST;
             building++) {
            if (castle->m_buildings & (1 << building)) {
                if (building == TOWN_UPGRADE_BUILDING_LAST)
                    castle->m_buildings &=
                        ~(IDX(TOWN_BUILDING_DWELLING_6)
                          | IDX(TOWN_BUILDING_UPGRADED_DWELLING_6));
                else
                    castle->m_buildings &=
                        -1 - (1 << (building - TOWN_UPGRADE_TO_DWELLING_OFFSET));
            }
        }
        for (building = TOWN_DWELLING_BUILDING_FIRST;
             building <= TOWN_DWELLING_BUILDING_LAST;
             building++) {
            if (castle->m_buildings & (1 << building)) {
                castle->m_garrison[building - TOWN_DWELLING_BUILDING_FIRST] =
                    gMonsterDatabase[IDX(
                        gDwellingType[IDX(castle->m_type)]
                                      [building - TOWN_DWELLING_BUILDING_FIRST]
                    )]
                        .growth;
            }
        }
        if (castle->m_buildings & IDX(TOWN_BUILDING_MAGE_GUILD)) {
            for (slot = 1; slot <= castle->m_buildState; slot++) {
                castle->m_spellCounts[slot] = gSpellLimits[slot - 1];
                if (castle->m_type == FACTION_WIZARD
                    && (castle->m_buildings & BIT(BUILDING_SLOT_SPECIAL)))
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
            for (spellSlot = 0; spellSlot < TOWN_MAGE_GUILD_SPELLS_PER_LEVEL; spellSlot++)
                castle->m_spellSlots[spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellSlot] =
                    SPELL_NONE;
        }

        // Fixed spell choices, selection weights, and retry limits are retail
        // mage-guild balance payload.
        // NOLINTBEGIN(readability-magic-numbers)
        if (castle->m_type == FACTION_NECROMANCER && castle->m_owner != -1
            && !gbHumanPlayer[castle->m_owner]) {
            if (Random(0, 100) < 50)
                spell = SPELL_DEATH_RIPPLE;
            else
                spell = SPELL_DEATH_WAVE;
            spellLevel = IDX(gsSpellInfo[IDX(spell)].level) - 1;
            castle->m_spells[spellLevel][spellsPerLevel[spellLevel]] = spell;
            spellsPerLevel[spellLevel]++;
        }

        roll = Random(0, 100);
        if (roll < 25)
            spell = SPELL_DISPEL;
        else if (roll < 50)
            spell = SPELL_MASS_DISPEL;
        else if (roll < 75)
            spell = SPELL_ANTI_MAGIC;
        else
            spell = SPELL_CURE;
        spellLevel = IDX(gsSpellInfo[IDX(spell)].level) - 1;
        castle->m_spells[spellLevel][spellsPerLevel[spellLevel]] = spell;
        spellsPerLevel[spellLevel]++;

        roll = Random(0, 100);
        if (roll < 20)
            spell = SPELL_MAGIC_ARROW;
        else if (roll < 40)
            spell = SPELL_LIGHTNING_BOLT;
        else if (roll < 60)
            spell = SPELL_FIREBALL;
        else if (roll < 80)
            spell = SPELL_COLD_RAY;
        else
            spell = SPELL_COLD_RING;
        spellLevel = IDX(gsSpellInfo[IDX(spell)].level) - 1;
        castle->m_spells[spellLevel][spellsPerLevel[spellLevel]] = spell;
        spellsPerLevel[spellLevel]++;

        for (spellLevel = 0; spellLevel < TOWN_MAGE_GUILD_LEVEL_COUNT; spellLevel++) {
            combatSpells = 0;
            for (spellSlot = 0; spellSlot < TOWN_MAGE_GUILD_SPELLS_PER_LEVEL; spellSlot++) {
                if (castle->m_spellSlots[spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellSlot]
                    != SPELL_NONE) {
                    usedSpells[IDX(
                        castle->m_spellSlots
                            [spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellSlot]
                    )] = 1;
                } else {
                    attempts = 0;
                    do {
                        spell =
                            SpellType(Random(IDX(SPELL_FIREBALL), IDX(SPELL_SET_WATER_GUARDIAN)));
                        while (IDX(gsSpellInfo[IDX(spell)].level) - 1 != spellLevel)
                            spell = SpellType(
                                Random(IDX(SPELL_FIREBALL), IDX(SPELL_SET_WATER_GUARDIAN))
                            );
                        if (castle->m_owner != -1 && !gbHumanPlayer[castle->m_owner])
                            spellValue =
                                (HAS(gsSpellInfo[IDX(spell)].attributes, SPELL_INFO_ATTRIBUTE_POWER)
                                     ? 4
                                     : 1)
                                    * gsSpellInfo[IDX(spell)].aiValue
                                + 50;
                        else
                            spellValue = 1500;
                        if (spell == SPELL_DIMENSION_DOOR)
                            spellValue = 1500;
                    } while (
                        (combatSpells == 1
                         && HAS(gsSpellInfo[IDX(spell)].attributes, SPELL_INFO_ATTRIBUTE_ADVENTURE))
                        || gsSpellInfo[IDX(spell)].raceChance[IDX(castle->m_type)] < Random(0, 10)
                        || attempts++ > 500 || usedSpells[IDX(spell)]
                        || spellValue < Random(1, 1500)
                    );
                    if (HAS(gsSpellInfo[IDX(spell)].attributes, SPELL_INFO_ATTRIBUTE_ADVENTURE))
                        combatSpells++;
                    castle
                        ->m_spellSlots[spellLevel * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL + spellSlot] =
                        spell;
                    usedSpells[IDX(spell)] = 1;
                }
            }
        }
        // NOLINTEND(readability-magic-numbers)
        H2_FREE_AT(ppMapExtra[extraIndex], DATA_COMPGEN(0x004f7570, setupTownsSourceFile, RETAIL_FILE), 6375);
        ppMapExtra[extraIndex] = NULL;
    }
}

VA(0x00482547, 0x774)
void game::ProcessOnMapHeroes(void) {
    DATA(0x004f7598) static i16 processOnMapHeroesSourceLineBase = 0x18ef;
    u32 extraIndex0;
    i32 pass19;
    i8 usedHeroes4[GAME_HERO_COUNT];
    i8 isJail6;
    hero* mapHero0;
    mapCell* cell5;
    mapHeroExtra* extra0;
    mapCell* townCell1;
    i32 heroId1;
    i32 armySlot0;
    i32 mapY15;
    i32 recordPosition;
    i32 mapX0;
    i32 townId4;
    FactionType heroClass6;
    i32 owner1;
    town* occupiedTown4;

    memset(usedHeroes4, 0, GAME_HERO_COUNT);
    for (pass19 = 0; pass19 < MAP_HERO_PROCESS_PASS_COUNT; pass19++) {
        for (mapY15 = 0; mapY15 < MAP_HEIGHT; mapY15++) {
            for (mapX0 = 0; mapX0 < MAP_WIDTH; mapX0++) {
                cell5 = &WORLDMAP->Row(mapY15)[mapX0];
                if ((cell5->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_HERO
                    || cell5->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_JAIL)) {

                    if ((cell5->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_JAIL)
                        isJail6 = 1;
                    else
                        isJail6 = 0;
                    extraIndex0 = cell5->m_objectMetadata;
                    extra0 = reinterpret_cast<mapHeroExtra*>(ppMapExtra[extraIndex0]);

                    if (pass19 == MAP_HERO_ASSIGNMENT_PASS) {
                        if (extra0->hasCustomHero && extra0->heroId < GAME_HERO_COUNT
                            && !usedHeroes4[extra0->heroId]) {
                            usedHeroes4[extra0->heroId] = 1;
                            extra0->hasAssignedHero = 1;
                        } else {
                            extra0->hasAssignedHero = 0;
                        }
                        if (isJail6) {
                            extra0->owner = -1;
                        } else {
                            extra0->owner =
                                static_cast<i8>(cell5->m_objectIndex / MAP_HERO_FRAME_STRIDE);
                            owner1 = gcColorToPlayerPos[extra0->owner];
                            extra0->owner = static_cast<i8>(owner1);
                        }
                    }

                    if (pass19 == MAP_HERO_CLASS_PASS) {
                        if (isJail6) {
                            heroClass6 = extra0->heroClass;
                        } else {
                            heroClass6 = static_cast<FactionType>(
                                cell5->m_objectIndex % MAP_HERO_FRAME_STRIDE
                            );
                            if (heroClass6 == FACTION_NEUTRAL) {
                                heroClass6 = m_setupPlayerRace
                                    [gcColorToSetupPos[gpGame->m_players[extra0->owner].m_color]];
                            }
                        }

                        if (extra0->hasAssignedHero) {
                            mapHero0 = GetHero(extra0->heroId);
                            mapHero0->m_cursorType = heroClass6;
                        } else {
                            heroId1 = RandomScan(
                                usedHeroes4,
                                IDX(heroClass6) * MAP_HEROES_PER_FACTION,
                                MAP_HEROES_PER_FACTION,
                                MAP_HERO_CLASS_SCAN_RETRY_LIMIT,
                                0
                            );
                            if (heroId1 == -1) {
                                heroId1 = RandomScan(
                                    usedHeroes4,
                                    0,
                                    GAME_HERO_COUNT,
                                    RANDOM_SCAN_RETRY_LIMIT,
                                    0
                                );
                                heroClass6 =
                                    static_cast<FactionType>(heroId1 / MAP_HEROES_PER_FACTION);
                            }
                            usedHeroes4[heroId1] = 1;
                            mapHero0 = GetHero(heroId1);
                            mapHero0->m_cursorType = heroClass6;
                            if (extra0->hasCustomHero && extra0->heroId >= GAME_HERO_COUNT)
                                mapHero0->m_portrait = extra0->heroId;
                            extra0->heroId = static_cast<i8>(heroId1);
                        }
                    }

                    if (pass19 == MAP_HERO_PLACEMENT_PASS) {
                        mapHero0 = GetHero(extra0->heroId);
                        if (!isJail6 && extra0->hasPatrol) {
                            mapHero0->m_patrolX = static_cast<i8>(mapX0);
                            mapHero0->m_patrolY = static_cast<i8>(mapY15);
                            mapHero0->m_patrolRadius = extra0->patrolRadius;
                        }
                        if (extra0->hasCustomArmy) {
                            for (armySlot0 = 0; armySlot0 < EVENT_RECORD_ARMY_SLOT_COUNT;
                                 armySlot0++) {
                                mapHero0->m_army.m_troopCounts[armySlot0] =
                                    extra0->troopCounts[armySlot0];
                                if (static_cast<i16>(mapHero0->m_army.m_troopCounts[armySlot0]) > 0)
                                    mapHero0->m_army.m_troopTypes[armySlot0] =
                                        extra0->troopTypes[armySlot0];
                                else
                                    mapHero0->m_army.m_troopTypes[armySlot0] = CREATURE_NONE;
                            }
                        }
                        for (recordPosition = 0;
                             recordPosition < EVENT_RECORD_HERO_ARTIFACT_COUNT;
                             recordPosition++) {
                            if (extra0->artifacts[recordPosition] >= 0)
                                GiveArtifact(
                                    mapHero0,
                                    ArtifactType(extra0->artifacts[recordPosition]),
                                    1,
                                    -1
                                );
                        }
                        if (extra0->hasCustomName)
                            strcpy(mapHero0->m_name, extra0->name);
                        mapHero0->m_experience = 0;
                        gpAdvManager->GiveExperience(mapHero0, extra0->experience, 1);
                        mapHero0->CheckLevel();
                        mapHero0->m_x = mapX0;
                        mapHero0->m_y = mapY15;

                        if (isJail6) {
                            mapHero0->m_owner = -1;
                            m_availableHeroes[extra0->heroId] = HERO_AVAILABILITY_JAILED;
                        } else {
                            mapHero0->m_owner = extra0->owner;
                            m_availableHeroes[extra0->heroId] = mapHero0->m_owner;
                            m_players[mapHero0->m_owner]
                                .m_heroIds[m_players[mapHero0->m_owner].m_heroCount] =
                                mapHero0->m_id;
                            m_players[mapHero0->m_owner].m_heroCount++;
                        }

                        if (!isJail6 && mapY15 > 0) {
                            townCell1 = &WORLDMAP->Row(mapY15 - 1)[mapX0];
                            if (townCell1->m_triggerType
                                == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
                                mapHero0->m_patrolY--;
                                mapHero0->m_y--;
                                townId4 = GetTownId(mapX0, mapY15 - 1);
                                occupiedTown4 = GetTown(townId4);
                                occupiedTown4->m_occupyingHeroId = mapHero0->m_id;
                            }
                        }

                        if (isJail6) {
                            cell5->m_objectMetadata = extra0->heroId;
                        } else {
                            cell5->m_objectTileset = TILESET_NONE;
                            cell5->m_objectIndex = MAPCELL_SPRITE_NONE;
                            cell5->m_objectMetadata = 0;
                            cell5->m_triggerType = 0;
                        }

                        if (extra0->hasCustomSkills) {
                            mapHero0->m_secondarySkillCount = 0;
                            for (recordPosition = 0;
                                 recordPosition < IDX(HERO_SKILL_COUNT);
                                 recordPosition++) {
                                mapHero0->m_secondarySkills[recordPosition] =
                                    HERO_SKILL_LEVEL_NONE;
                                mapHero0->m_secondarySkillOrder[recordPosition] = 0;
                            }
                            for (recordPosition = 0;
                                 recordPosition < EVENT_RECORD_SKILL_CAPACITY;
                                 recordPosition++) {
                                if (extra0->skillTypes[recordPosition] != -1) {
                                    mapHero0->GiveSS(
                                        static_cast<HeroSecondarySkill>(
                                            extra0->skillTypes[recordPosition]
                                        ),
                                        static_cast<HeroSkillLevel>(
                                            extra0->skillLevels[recordPosition]
                                        )
                                    );
                                }
                            }
                        }
                        if (!isJail6) {
                            SetVisibility(
                                mapHero0->m_x,
                                mapHero0->m_y,
                                mapHero0->m_owner,
                                giVisRange[IDX(mapHero0->m_secondarySkills
                                                   [MAP_HERO_SCOUTING_SKILL_INDEX])]
                            );
                        }
                        H2_FREE_AT(
                            ppMapExtra[extraIndex0],
                            DATA_COMPGEN(
                                0x004f759c,
                                processOnMapHeroesSourceFile,
                                RETAIL_FILE
                            ),
                            processOnMapHeroesSourceLineBase + 0xdd
                        );
                        ppMapExtra[extraIndex0] = NULL;
                    }
                }
            }
        }
    }
}

VA(0x00482cbb, 0x55e)
void game::CheckHeroConsistency(void) {
    hero* mapHero3;
    mapCell* cell1;
    i32 x11;
    i32 y8;
    i32 player3;
    i32 slot1;
    i32 total26 = 0;
    i32 consistent13;
    town* occupiedTown9;

    for (player3 = 0; player3 < m_playerCount; player3++) {
        if (m_playerDead[player3] != 0)
            continue;
        total26 += m_players[player3].m_heroCount;
        for (slot1 = 0; slot1 < m_players[player3].m_heroCount; slot1++) {
            if (m_heroRecs[m_players[player3].m_heroIds[slot1]].m_owner != player3)
                consistent13 = 0;
        }
    }

    for (player3 = 0; player3 < m_playerCount; player3++) {
        if (m_playerDead[player3] == 0) {
            for (slot1 = 0; slot1 < AVAILABLE_HERO_SLOTS; slot1++) {
                if ((m_availableHeroes[m_players[player3].m_availableHeroIds[slot1]] >= 0
                     && m_availableHeroes[m_players[player3].m_availableHeroIds[slot1]]
                            <= HERO_CONSISTENCY_PLAYABLE_FACTION_MAX)
                    || (total26 < HERO_CONSISTENCY_POOL_THRESHOLD
                        && m_availableHeroes[m_players[player3].m_availableHeroIds[slot1]] == -1)) {
                    m_players[player3].m_availableHeroIds[slot1] =
                        static_cast<i8>(GetNewHeroId(player3, FACTION_ANY, 0));
                    m_availableHeroes[m_players[player3].m_availableHeroIds[slot1]] =
                        WEEKLY_AVAILABLE_HERO;
                }
            }
        }
    }

    for (x11 = 0; x11 < MAP_WIDTH; x11++) {
        for (y8 = 0; y8 < MAP_HEIGHT; y8++) {
            cell1 = gpAdvManager->GetCell(x11, y8);
            if (cell1->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MERMAID)) {
                if (cell1->m_objectMetadata >= 0 && cell1->m_objectMetadata < GAME_HERO_COUNT) {
                    mapHero3 = GetHero(cell1->m_objectMetadata);
                    if (mapHero3->m_x != x11 || mapHero3->m_y != y8) {
                        cell1->m_triggerType = 0;
                        cell1->m_objectMetadata = 0;
                    }
                    if (mapHero3->m_owner < 0 || mapHero3->m_owner >= GAME_PLAYER_COUNT) {
                        if (mapHero3->m_locationType
                            == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
                            occupiedTown9 = gpGame->GetTown(mapHero3->m_occupiedTown);
                            occupiedTown9->m_occupyingHeroId = -1;
                        }
                        if (mapHero3->m_x == x11 && mapHero3->m_y == y8) {
                            RestoreCell(
                                mapHero3->m_x,
                                mapHero3->m_y,
                                mapHero3->m_locationType,
                                mapHero3->m_occupiedTown,
                                NULL,
                                1
                            );
                        } else {
                            cell1->m_triggerType = 0;
                            cell1->m_objectMetadata = 0;
                        }
                    }
                } else {
                    cell1->m_triggerType = 0;
                }
            }
        }
    }

    for (player3 = 0; player3 < GAME_HERO_COUNT; player3++) {
        for (slot1 = 0; slot1 < ARMY_GROUP_SLOT_COUNT; slot1++) {
            if (m_heroRecs[player3].m_army.m_troopTypes[slot1] == CREATURE_NONE
                || m_heroRecs[player3].m_army.m_creatureCounts[slot1] < 0)
                m_heroRecs[player3].m_army.m_creatureCounts[slot1] = 0;
        }
    }
    for (player3 = 0; player3 < GAME_TOWN_COUNT; player3++) {
        for (slot1 = 0; slot1 < ARMY_GROUP_SLOT_COUNT; slot1++) {
            if (m_castleRecs[player3].m_army.m_troopTypes[slot1] == CREATURE_NONE
                || m_castleRecs[player3].m_army.m_creatureCounts[slot1] < 0)
                m_castleRecs[player3].m_army.m_creatureCounts[slot1] = 0;
        }
    }
}

#define done done36
#define fileData fileData9
#define chunkSize chunkSize5
#define fileSize fileSize6
#define result result29
#define fileCrc fileCrc29
#define acknowledged acknowledged4
#define reply reply6
#define transmitCrc transmitCrc18
#define packetsInBatch packetsInBatch2
#define header header3
#define batch batch12
#define file file37
#define oldTrack oldTrack12
#define packetCount packetCount14
#define packet packet6
#define batchCount batchCount29
#define transmitData transmitData3
#define filename filename8
#define samplesReady samplesReady1
#define success success14

VA(0x00483219, 0x71e)
i32 game::TransmitSaveGame(i32 remotePlayer, i32 player, i32 useCurrentSave) {
    i32 success;
    i32 samplesReady;
    char filename[TRANSMIT_FILENAME_CAPACITY];
    u8* transmitData;
    i32 batchCount;
    i32 packet;
    i32 packetCount;
    i32 unused484;
    i32 oldTrack;
    i32 file;
    i32 unused3c12;
    i32 batch;
    i32* header;
    i32 packetsInBatch;
    u32 transmitCrc;
    char* reply;
    char* acknowledged;
    u32 fileCrc;
    i32 unused1c3;
    i32 result;
    i32 unused140;
    i32 fileSize;
    i32 chunkSize;
    u8* fileData;
    i32 done;

    gpAdvManager->TrimLoopingSounds(REMOTE_LOOPING_SOUND_COUNT);
    header = NULL;
    reply = NULL;
    transmitData = NULL;
    fileData = NULL;
    success = 0;
    result = 0;
    acknowledged = NULL;
    oldTrack = -1;

    samplesReady = gpSoundManager->m_samplesReady;
    gpSoundManager->m_samplesReady = 1;
    oldTrack = static_cast<i8>(gpSoundManager->m_currentTrack);
    gpSoundManager->SwitchAmbientMusic(-1);
    gpSoundManager->m_samplesReady = samplesReady;

    LogStr(const_cast<char*>(DATA_COMPGEN(0x004f75c8, transmitSaveGameTransmitGameStart, "Transmit Game Start")));
    if (gpAdvManager->m_active == 1)
        BVResMsg(const_cast<char*>(DATA_COMPGEN(0x004f75dc, transmitSaveGameSendingData, "Sending Data")), RES_NONE, 0);
    AiPrint(const_cast<char*>(DATA_COMPGEN(0x004f75ec, transmitSaveGameTransmitStartCompressing, "Transmit Start - Compressing")));

    acknowledged = static_cast<char*>(H2_ALLOC_AT(REMOTE_PACKET_TRACKING_CAPACITY, DATA_COMPGEN(0x004f760c, transmitSaveGameSourceFile, RETAIL_FILE), gTransmitSourceLine + 0x2b));
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
    sprintf(filename, DATA_COMPGEN(0x004f763c, transmitSaveGameSS, "%s%s"), DATA_COMPGEN(0x004f7634, transmitSaveGameDATA, ".\\DATA\\"), gConfig.rmtSDName);
    fileSize = FileSize(filename);
    LogInt(
        const_cast<char*>(DATA_COMPGEN(0x004f7644, transmitSaveGamePostDiffFileSize, "PostDiffFileSize")),
        fileSize,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );

    header = static_cast<i32*>(H2_ALLOC_AT(REMOTE_HEADER_CAPACITY, DATA_COMPGEN(0x004f7658, transmitSaveGameSourceFile2, RETAIL_FILE), gTransmitSourceLine + 0x3f));
    if (gbUseRegularCompression)
        transmitData = static_cast<u8*>(H2_ALLOC_AT(fileSize + REMOTE_BUFFER_EXTRA, DATA_COMPGEN(0x004f7680, transmitSaveGameSourceFile3, RETAIL_FILE), gTransmitSourceLine + 0x41));
    fileData = static_cast<u8*>(H2_ALLOC_AT(fileSize + REMOTE_BUFFER_EXTRA, DATA_COMPGEN(0x004f76a8, transmitSaveGameSourceFile4, RETAIL_FILE), gTransmitSourceLine + 0x42));

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

        AiPrint(const_cast<char*>(DATA_COMPGEN(0x004f76d0, transmitSaveGameTransmitStartSending, "Transmit Start - Sending")));
        if (gbUseRegularCompression)
            transmitCrc = calc_crc_long(transmitData, fileSize);
        else
            transmitCrc = fileCrc;
        LogInt(
            const_cast<char*>(DATA_COMPGEN(0x004f76ec, transmitSaveGameSend, "Send")),
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
                LogStr(const_cast<char*>(DATA_COMPGEN(0x004f76f4, transmitSaveGamePreWait, "PreWait")));
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
                LogStr(const_cast<char*>(DATA_COMPGEN(0x004f76fc, transmitSaveGamePostWait, "PostWait")));
                if (!result)
                    ShutDown(NULL);
                for (packet = 0; packet < packetsInBatch; packet++) {
                    if (reinterpret_cast<RemoteMessage*>(reply)->payload[packet] > 0)
                        acknowledged[batch * REMOTE_PACKET_BATCH_SIZE + packet] = 1;
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
        H2_FREE_AT(header, DATA_COMPGEN(0x004f7708, transmitSaveGameSourceFile5, RETAIL_FILE), gTransmitSourceLine + 0xc7);
    if (transmitData)
        H2_FREE_AT(transmitData, DATA_COMPGEN(0x004f7730, transmitSaveGameSourceFile6, RETAIL_FILE), gTransmitSourceLine + 0xc8);
    if (fileData && fileData != transmitData)
        H2_FREE_AT(fileData, DATA_COMPGEN(0x004f7758, transmitSaveGameSourceFile7, RETAIL_FILE), gTransmitSourceLine + 0xc9);
    if (acknowledged)
        H2_FREE_AT(acknowledged, DATA_COMPGEN(0x004f7780, transmitSaveGameSourceFile8, RETAIL_FILE), gTransmitSourceLine + 0xca);

    AiPrint(const_cast<char*>(DATA_COMPGEN(0x004f77a8, transmitSaveGameTransmitEnd, "Transmit End")));
    if (gpAdvManager->m_active == 1) {
        giBottomViewOverride = BOTTOM_VIEW_NONE;
        gpAdvManager->UpdBottomView(1, 1, 1);
    }
    if (oldTrack != -1) {
        samplesReady = gpSoundManager->m_samplesReady;
        gpSoundManager->m_samplesReady = 1;
        gpSoundManager->SwitchAmbientMusic(oldTrack);
        gpSoundManager->m_samplesReady = samplesReady;
    }
    return success;
}

#undef done
#undef fileData
#undef chunkSize
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
#undef batchCount
#undef transmitData
#undef filename
#undef samplesReady
#undef success

#define decodedData decodedData14
#define packetStart packetStart6
#define received received29
#define lastPacketTime lastPacketTime5
#define result result9
#define packet packet15
#define ackBuffer ackBuffer29
#define receivedCrc receivedCrc7
#define finished finished26
#define computedCrc computedCrc1
#define file file6
#define oldTrack oldTrack36
#define index index1
#define incomingData incomingData36
#define filename filename7
#define samplesReady samplesReady0
#define success success15

VA(0x00483937, 0x68d)
i32 game::ReceiveSaveGame(
    i32 dataSize,
    i32 expectedCrc,
    i32 expectedTransmitCrc,
    i32 remotePlayer
) {
    i32 unused20819;
    i32 success;
    i32 samplesReady;
    char filename[RECEIVE_FILENAME_CAPACITY];
    u8* incomingData;
    i32 index;
    i32 oldTrack;
    i32 file;
    i32 computedCrc;
    i32 finished;
    i32 receivedCrc;
    u8* ackBuffer;
    RemoteMessage* packet;
    i32 result;
    i32l lastPacketTime;
    char* received;
    i32 packetStart;
    u8* decodedData;

    LogInt(
        const_cast<char*>(DATA_COMPGEN(0x004f77bc, receiveSaveGameFW1, "FW1")),
        remotePlayer,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );
    LogStr(const_cast<char*>(DATA_COMPGEN(0x004f77c0, receiveSaveGameRSG1, "RSG1")));
    AiPrint(const_cast<char*>(DATA_COMPGEN(0x004f77c8, receiveSaveGameReceiveStartGettingData, "Receive Start - Getting Data")));
    gpAdvManager->TrimLoopingSounds(REMOTE_LOOPING_SOUND_COUNT);

    ackBuffer = NULL;
    incomingData = NULL;
    decodedData = NULL;
    packet = NULL;
    file = 0;
    finished = 0;
    unused20819 = 0;
    received = NULL;
    success = 0;
    oldTrack = -1;

    gpAdvManager->UnwindMapChangeQueue(REMOTE_MAP_CHANGE_UNWIND_LIMIT, 0);
    if (gpAdvManager->m_active == 1)
        BVResMsg(const_cast<char*>(DATA_COMPGEN(0x004f77e8, receiveSaveGameReceivingData, "Receiving Data")), RES_NONE, 0);

    samplesReady = gpSoundManager->m_samplesReady;
    oldTrack = static_cast<i8>(gpSoundManager->m_currentTrack);
    gpSoundManager->m_samplesReady = 1;
    gpSoundManager->SwitchAmbientMusic(-1);
    gpSoundManager->m_samplesReady = samplesReady;

    LogStr(const_cast<char*>(DATA_COMPGEN(0x004f77f8, receiveSaveGameBeginTransmitInitConfirm, "Begin Transmit Init Confirm")));
    result = TransmitRemoteData(
        NULL,
        remotePlayer,
        0,
        REMOTE_SAVE_INIT_RESPONSE,
        1,
        1,
        REMOTE_MESSAGE_DEFAULT
    );
    LogStr(const_cast<char*>(DATA_COMPGEN(0x004f7814, receiveSaveGameEndTransmitInitConfirm, "End Transmit Init Confirm")));
    if (!result)
        ShutDown(NULL);

    received = static_cast<char*>(H2_ALLOC_AT(REMOTE_PACKET_TRACKING_CAPACITY, DATA_COMPGEN(0x004f7830, receiveSaveGameSourceFile, RETAIL_FILE), gReceiveSourceLine + 0x33));
    memset(received, 0, REMOTE_PACKET_TRACKING_CAPACITY);
    if (gbUseRegularCompression)
        decodedData = static_cast<u8*>(H2_ALLOC_AT(REMOTE_DECODE_BUFFER_SIZE, DATA_COMPGEN(0x004f7858, receiveSaveGameSourceFile2, RETAIL_FILE), gReceiveSourceLine + 0x37));
    ackBuffer = static_cast<u8*>(H2_ALLOC_AT(REMOTE_HEADER_CAPACITY, DATA_COMPGEN(0x004f7880, receiveSaveGameSourceFile3, RETAIL_FILE), gReceiveSourceLine + 0x39));
    incomingData = static_cast<u8*>(H2_ALLOC_AT(dataSize + REMOTE_BUFFER_EXTRA, DATA_COMPGEN(0x004f78a8, receiveSaveGameSourceFile4, RETAIL_FILE), gReceiveSourceLine + 0x3a));

    lastPacketTime = KBTickCount();
    LogInt(
        const_cast<char*>(DATA_COMPGEN(0x004f78d0, receiveSaveGameFW2, "FW2")),
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
        if (KBTickCount() > lastPacketTime + REMOTE_RECEIVE_TIMEOUT) {
            NormalDialog(
                const_cast<char*>(DATA_COMPGEN(0x004f78d4, receiveSaveGameErrorReceivingDataKeepTrying, "Error receiving data.  Keep trying?")),
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
                    LogInt(
                        const_cast<char*>(DATA_COMPGEN(0x004f78f8, receiveSaveGameFW3, "FW3")),
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

    AiPrint(const_cast<char*>(DATA_COMPGEN(0x004f78fc, receiveSaveGameReceiveStartDecompressingData, "Receive Start - Decompressing Data")));
    receivedCrc = calc_crc_long(incomingData, dataSize);
    LogInt(
        const_cast<char*>(DATA_COMPGEN(0x004f7920, receiveSaveGameReceive, "Receive")),
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
        const_cast<char*>(DATA_COMPGEN(0x004f7928, receiveSaveGameReceive2, "Receive")),
        dataSize,
        computedCrc,
        expectedCrc,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );

    sprintf(filename, DATA_COMPGEN(0x004f7938, receiveSaveGameSS, "%s%s"), DATA_COMPGEN(0x004f7930, receiveSaveGameDATA, ".\\DATA\\"), gConfig.rmtRDName);
    file = open(filename, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (file == -1)
        FileError(filename);
    write(file, decodedData, dataSize);
    close(file);
    success = 1;

    if (received)
        H2_FREE_AT(received, DATA_COMPGEN(0x004f7940, receiveSaveGameSourceFile5, RETAIL_FILE), gReceiveSourceLine + 0xa1);
    if (ackBuffer)
        H2_FREE_AT(ackBuffer, DATA_COMPGEN(0x004f7968, receiveSaveGameSourceFile6, RETAIL_FILE), gReceiveSourceLine + 0xa2);
    if (incomingData)
        H2_FREE_AT(incomingData, DATA_COMPGEN(0x004f7990, receiveSaveGameSourceFile7, RETAIL_FILE), gReceiveSourceLine + 0xa3);
    if (decodedData && incomingData != decodedData)
        H2_FREE_AT(decodedData, DATA_COMPGEN(0x004f79b8, receiveSaveGameSourceFile8, RETAIL_FILE), gReceiveSourceLine + 0xa4);

    CreateJoinFile(gConfig.rmtRLName, gConfig.rmtRDName, gConfig.rmtRCName);
    AiPrint(const_cast<char*>(DATA_COMPGEN(0x004f79e0, receiveSaveGameReceiveEnd, "Receive End")));
    if (gpAdvManager->m_active == 1) {
        giBottomViewOverride = BOTTOM_VIEW_NONE;
        gpAdvManager->UpdBottomView(1, 1, 1);
    }
    if (oldTrack != -1) {
        samplesReady = gpSoundManager->m_samplesReady;
        gpSoundManager->m_samplesReady = 1;
        gpSoundManager->SwitchAmbientMusic(oldTrack);
        gpSoundManager->m_samplesReady = samplesReady;
    }
    return success;
}

#undef decodedData
#undef packetStart
#undef received
#undef lastPacketTime
#undef result
#undef packet
#undef ackBuffer
#undef receivedCrc
#undef finished
#undef computedCrc
#undef file
#undef oldTrack
#undef index
#undef incomingData
#undef filename
#undef samplesReady
#undef success

VA(0x00483fc4, 0x455)
void game::DoNewTurn(void) {
    char musicFile18[NEW_TURN_MUSIC_FILENAME_CAPACITY];
    char lowerName19[NEW_TURN_LOWER_NAME_CAPACITY];
    i32 musicTrack2;

    CheckForTimeEvent();
    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        CheckEndGame(END_GAME_FORCE_NONE, false);
        return;
    }
    giBottomViewOverrideEndTime = KBTickCount() + NEW_TURN_BOTTOM_VIEW_DURATION;
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
            gpSoundManager->m_samplesReady = 1;
        if (giWeekType != CALENDAR_PERIOD_NONE) {
            musicTrack2 = -1;
            if (m_week == 1) {
                musicTrack2 = NEW_MONTH_MUSIC_TRACK;
                strcpy(musicFile18, DATA_COMPGEN(0x004f79ec, doNewTurnNewmonth82m, "newmonth.82m"));
                if (giMonthType == CALENDAR_PERIOD_NORMAL) {
                    sprintf(
                        gText,
                        cNewTurn[NEW_MONTH_NORMAL_TEXT],
                        gMonthNames[giMonthTypeExtra]
                    );
                } else if (giMonthType == CALENDAR_PERIOD_CREATURE) {
                    strcpy(lowerName19, gArmyNames[giMonthTypeExtra]);
                    lowerName19[0] -= 'a' - 'A';
                    sprintf(
                        gText,
                        cNewTurn[NEW_MONTH_CREATURE_TEXT],
                        gArmyNames[giMonthTypeExtra],
                        lowerName19
                    );
                } else {
                    sprintf(gText, cNewTurn[NEW_MONTH_PLAGUE_TEXT]);
                }
            } else {
                musicTrack2 = NEW_WEEK_MUSIC_TRACK;
                strcpy(musicFile18, DATA_COMPGEN(0x004f79fc, doNewTurnNewweek82m, "newweek.82m"));
                if (giWeekType == CALENDAR_PERIOD_NORMAL) {
                    sprintf(gText, cNewTurn[NEW_WEEK_NORMAL_TEXT], gWeekNames[giWeekTypeExtra]);
                } else {
                    strcpy(lowerName19, gArmyNames[giWeekTypeExtra]);
                    lowerName19[0] -= 'a' - 'A';
                    sprintf(
                        gText,
                        cNewTurn[NEW_WEEK_CREATURE_TEXT],
                        gArmyNames[giWeekTypeExtra],
                        lowerName19
                    );
                }
            }
            gpSoundManager->PlayAmbientMusic(musicTrack2, 0, -1);
            gpMouseManager->SetPointer(0);
            NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
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

VA(0x00484419, 0x58)
i32 game::GetBoatsBuilt(void) {
    i32 count = 0;
    i32 i;
    for (i = 0; i < GAME_BOAT_COUNT; i++) {
        if (m_boatSlots[i] != -1)
            count++;
    }
    return count;
}

VA(0x00484471, 0x9c)
i32 game::GetNumThievesGuilds(i32 color) {
    i32 num = 0;
    i32 i;
    for (i = 0; i < m_players[color].m_townCount; i++) {
        if (gpGame->m_castleRecs[m_players[color].m_townIds[i]].m_buildings
            & IDX(TOWN_BUILDING_THIEVES_GUILD))
            num++;
    }
    return num;
}

VA(0x0048450d, 0x113)
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
        ;
    else if (m_mapHeader.difficulty == DIFFICULTY_NORMAL)
        rating += MAP_RATING_NORMAL_BONUS;
    else if (m_mapHeader.difficulty == DIFFICULTY_HARD)
        rating += MAP_RATING_HARD_BONUS;
    else if (m_mapHeader.difficulty == DIFFICULTY_EXPERT)
        rating += MAP_RATING_EXPERT_BONUS;
    return rating;
}

VA(0x00484620, 0x1ea)
i32 CalcBaseScore(i32 days) {
    i32 score = SCORE_BASE;

    if (gpGame->m_mapHeader.width == MAP_DIMENSION_XLARGE)
        days = static_cast<i32>(days * GAME_SCORE_EXTRA_LARGE_DAY_SCALE);
    else if (gpGame->m_mapHeader.width == MAP_DIMENSION_LARGE)
        days = static_cast<i32>(days * GAME_SCORE_LARGE_DAY_SCALE);
    else if (gpGame->m_mapHeader.width == MAP_DIMENSION_MEDIUM) {
    } else if (gpGame->m_mapHeader.width == MAP_DIMENSION_SMALL)
        days = static_cast<i32>(days * GAME_SCORE_SMALL_DAY_SCALE);

    if (days <= SCORE_FIRST_TIER) {
        score -= -(-days);
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
            score - (days - SCORE_SECOND_TIER) * DATA_COMPGEN(0x004eb770, calcBaseScoreConstant, SCORE_THIRD_TIER_FACTOR)
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

VA(0x0048480a, 0xb5)
void game::RestoreCell(
    i32 x,
    i32 y,
    H2_ENUM_PARAM(MapObjectType, i32) objectType,
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

VA(0x004848bf, 0xe3)
void game::SetMapSize(i32 w, i32 h) {
    DATA(0x004f7a0c) static i16 setMapSizeSourceLineBase = 0x1d0d;
    if (h == MAP_HEIGHT && w == MAP_WIDTH && bMapInitialized)
        goto mapSized;
    {
        bMapInitialized = 1;
        MAP_WIDTH = w;
        MAP_HEIGHT = h;
        gpSearchArray->Init();
    }
mapSized:
    if (mapExtra)
        H2_FREE_AT(mapExtra, DATA_COMPGEN(0x004f7a10, setMapSizeSourceFile, RETAIL_FILE), setMapSizeSourceLineBase + 12);
    mapExtra = static_cast<u8*>(H2_ALLOC_AT(MAP_WIDTH * MAP_HEIGHT, DATA_COMPGEN(0x004f7a38, setMapSizeSourceFile2, RETAIL_FILE), setMapSizeSourceLineBase + 13));
    memset(mapExtra, 0, MAP_WIDTH * MAP_HEIGHT);
}

VA(0x004849a2, 0x100)
void WriteDiffHeaderInfo(u8 cmd, i32 len, u8* buf, i32* pos) {
    u8 flags = 0;
    flags = (cmd << COMMAND_SHIFT) | flags;
    if (len > LEN_WORD_MAX) {
        flags |= LEN_WORD_FLAG;
        flags |= (len & LEN_HIGH_MASK) >> DIFF_WORD_SHIFT;
        u16 word = static_cast<u16>(len & LEN_LOW_MASK);
        buf[*pos] = flags;
        *reinterpret_cast<u16*>(buf + *pos + 1) = word;
        *pos += DIFF_WORD_HEADER_SIZE;
    } else if (len > LEN_BYTE_MAX) {
        flags |= LEN_BYTE_FLAG;
        flags |= (len >> DIFF_BYTE_SHIFT) & LEN_SHORT_MASK;
        u8 lo = static_cast<u8>(len);
        buf[*pos] = flags;
        buf[*pos + 1] = lo;
        *pos += DIFF_BYTE_HEADER_SIZE;
    } else {
        flags |= static_cast<u8>(len);
        buf[*pos] = flags;
        (*pos)++;
    }
}

VA(0x00484aa2, 0xab)
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

VA(0x00484b4d, 0x5ba)
void CreateDiffFile(
    char* oldName,
    char* joinName,
    char* diffName,
    i32 remotePlayer,
    i32 forceWhole
) {
    u8* diffData6;
    i32 unusedFirst0;
    i32 unusedSecond4;
    u8* joinData29;
    i32 joinSize36;
    i32 joinFile1;
    i32 copyLength28;
    i32l startTime11;
    i32 diffSize29;
    u8* oldData13;
    i32 oldSize37;
    i32 compareOffset4;
    i32 position1;
    i32 sendWhole4;
    i32 oldFile17;

    startTime11 = KBTickCount();
    oldData13 = NULL;
    joinData29 = NULL;
    diffData6 = NULL;
    oldSize37 = 0;
    joinSize36 = 0;
    diffSize29 = 0;
    sendWhole4 = 0;

    if (forceWhole || (iLastDiffSendTo != -1 && remotePlayer != iLastDiffSendTo))
        sendWhole4 = 1;
    iLastDiffSendTo = remotePlayer;

    sprintf(gText, DATA_COMPGEN(0x004f7a6c, createDiffFileSS, "%s%s"), DATA_COMPGEN(0x004f7a64, createDiffFileDATA, ".\\DATA\\"), joinName);
    joinSize36 = FileSize(gText);
    joinData29 = static_cast<u8*>(H2_ALLOC_AT(joinSize36, DATA_COMPGEN(0x004f7a74, createDiffFileSourceFile, RETAIL_FILE), 7550));
    sprintf(gText, DATA_COMPGEN(0x004f7aa4, createDiffFileSS2, "%s%s"), DATA_COMPGEN(0x004f7a9c, createDiffFileDATA2, ".\\DATA\\"), joinName);
    joinFile1 = open(gText, _O_BINARY);
    if (joinFile1 == -1)
        FileError(gText);
    read(joinFile1, joinData29, joinSize36);
    close(joinFile1);
    LogInt(
        const_cast<char*>(DATA_COMPGEN(0x004f7aac, createDiffFileOrigJoinCRC, "Orig Join CRC")),
        calc_crc_long(joinData29, joinSize36),
        joinSize36,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );

    if (!forceWhole) {
        sprintf(gText, DATA_COMPGEN(0x004f7ac4, createDiffFileSS3, "%s%s"), DATA_COMPGEN(0x004f7abc, createDiffFileDATA3, ".\\DATA\\"), oldName);
        oldSize37 = FileSize(gText);
        oldData13 = static_cast<u8*>(H2_ALLOC_AT(oldSize37, DATA_COMPGEN(0x004f7acc, createDiffFileSourceFile2, RETAIL_FILE), 7571));
        sprintf(gText, DATA_COMPGEN(0x004f7afc, createDiffFileSS4, "%s%s"), DATA_COMPGEN(0x004f7af4, createDiffFileDATA4, ".\\DATA\\"), oldName);
        oldFile17 = open(gText, _O_BINARY);
        if (oldFile17 == -1)
            FileError(gText);
        read(oldFile17, oldData13, oldSize37);
        close(oldFile17);
    }

    diffData6 = static_cast<u8*>(H2_ALLOC_AT(
        (oldSize37 > joinSize36 ? oldSize37 : joinSize36) + DIFF_BUFFER_EXTRA, DATA_COMPGEN(0x004f7b04, createDiffFileSourceFile3, RETAIL_FILE),
        7581
    ));
    if (sendWhole4) {
        diffData6[0] = 0;
        diffData6[1] = 0;
        memcpy(diffData6 + JOIN_HEADER_SIZE, joinData29, joinSize36);
        diffSize29 = joinSize36 + JOIN_HEADER_SIZE;
    } else {
        diffData6[0] = 1;
        diffData6[1] = 0;
        diffSize29 = JOIN_HEADER_SIZE;
        position1 = 0;
        copyLength28 = 0;
        compareOffset4 = copyLength28;
        while (1) {
            if (position1 + copyLength28 >= oldSize37 || position1 + copyLength28 >= joinSize36) {
                copyLength28 = oldSize37 - position1;
                WriteDiffHeaderInfo(1, copyLength28, diffData6, &diffSize29);
                memcpy(diffData6 + diffSize29, joinData29 + position1, copyLength28);
                diffSize29 += copyLength28;
                position1 += copyLength28;
                copyLength28 = 0;
                break;
            }
            if (oldData13[position1 + copyLength28] == joinData29[position1 + copyLength28]) {
                compareOffset4 = 1;
                while (position1 + compareOffset4 + copyLength28 < oldSize37
                       && position1 + compareOffset4 + copyLength28 < joinSize36
                       && oldData13[position1 + compareOffset4 + copyLength28]
                              == joinData29[position1 + compareOffset4 + copyLength28])
                    compareOffset4++;
                if (compareOffset4 <= DIFF_MAX_SHORT_MATCH) {
                    copyLength28 += compareOffset4;
                    compareOffset4 = 0;
                    continue;
                } else {
                    if (copyLength28 != 0) {
                        WriteDiffHeaderInfo(1, copyLength28, diffData6, &diffSize29);
                        memcpy(diffData6 + diffSize29, joinData29 + position1, copyLength28);
                        diffSize29 += copyLength28;
                        position1 += copyLength28;
                        copyLength28 = 0;
                    }
                    WriteDiffHeaderInfo(0, compareOffset4, diffData6, &diffSize29);
                    position1 += compareOffset4;
                    compareOffset4 = 0;
                }
            } else {
                while (position1 + copyLength28 < oldSize37 && position1 + copyLength28 < joinSize36
                       && oldData13[position1 + copyLength28]
                              != joinData29[position1 + copyLength28])
                    copyLength28++;
            }
        }
    }

    sprintf(gText, DATA_COMPGEN(0x004f7b34, createDiffFileSS5, "%s%s"), DATA_COMPGEN(0x004f7b2c, createDiffFileDATA5, ".\\DATA\\"), diffName);
    joinFile1 = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (joinFile1 == -1)
        FileError(gText);
    write(joinFile1, diffData6, diffSize29);
    close(joinFile1);

    sprintf(gText, DATA_COMPGEN(0x004f7b44, createDiffFileSS6, "%s%s"), DATA_COMPGEN(0x004f7b3c, createDiffFileDATA6, ".\\DATA\\"), oldName);
    joinFile1 = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (joinFile1 == -1)
        FileError(gText);
    write(joinFile1, joinData29, joinSize36);
    close(joinFile1);

    if (oldData13 != NULL)
        H2_FREE_AT(oldData13, DATA_COMPGEN(0x004f7b4c, createDiffFileSourceFile4, RETAIL_FILE), 7687);
    if (joinData29 != NULL)
        H2_FREE_AT(joinData29, DATA_COMPGEN(0x004f7b74, createDiffFileSourceFile5, RETAIL_FILE), 7689);
    if (diffData6 != NULL)
        H2_FREE_AT(diffData6, DATA_COMPGEN(0x004f7b9c, createDiffFileSourceFile6, RETAIL_FILE), 7691);
    return;
}

VA(0x00485107, 0x3ce)
void CreateJoinFile(char* oldName, char* diffName, char* joinName) {
    DATA(0x004f7bc4) static i16 createJoinFileSourceLineBase = 0x1e0f;
    u8* oldData13 = NULL;
    u8* diffData5 = NULL;
    u8* joinData9 = NULL;
    i32 joinSize37 = 0;
    i32 diffSize1;
    i32 copyLength9;
    i32 diffFile2;
    i32 oldSize10;
    u8 copyFlag16;
    i32 position1;
    i32 joinFile0;

    sprintf(gText, DATA_COMPGEN(0x004f7bd0, createJoinFileSS, "%s%s"), DATA_COMPGEN(0x004f7bc8, createJoinFileDATA, ".\\DATA\\"), diffName);
    diffSize1 = FileSize(gText);
    diffData5 = static_cast<u8*>(H2_ALLOC_AT(diffSize1, DATA_COMPGEN(0x004f7bd8, createJoinFileSourceFile, RETAIL_FILE), createJoinFileSourceLineBase + 0xd));
    sprintf(gText, DATA_COMPGEN(0x004f7c08, createJoinFileSS2, "%s%s"), DATA_COMPGEN(0x004f7c00, createJoinFileDATA2, ".\\DATA\\"), diffName);
    diffFile2 = open(gText, _O_BINARY);
    if (diffFile2 == -1)
        FileError(gText);
    read(diffFile2, diffData5, diffSize1);
    close(diffFile2);

    joinData9 = static_cast<u8*>(H2_ALLOC_AT(JOIN_BUFFER_SIZE, DATA_COMPGEN(0x004f7c10, createJoinFileSourceFile2, RETAIL_FILE), createJoinFileSourceLineBase + 0x16));
    if (diffData5[0] == 0) {
        memcpy(joinData9, diffData5 + JOIN_HEADER_SIZE, diffSize1 - JOIN_HEADER_SIZE);
        joinSize37 = diffSize1 - JOIN_HEADER_SIZE;
    } else {
        sprintf(gText, DATA_COMPGEN(0x004f7c40, createJoinFileSS3, "%s%s"), DATA_COMPGEN(0x004f7c38, createJoinFileDATA3, ".\\DATA\\"), oldName);
        oldSize10 = FileSize(gText);
        oldData13 = static_cast<u8*>(H2_ALLOC_AT(oldSize10, DATA_COMPGEN(0x004f7c48, createJoinFileSourceFile3, RETAIL_FILE), createJoinFileSourceLineBase + 0x21));
        sprintf(gText, DATA_COMPGEN(0x004f7c78, createJoinFileSS4, "%s%s"), DATA_COMPGEN(0x004f7c70, createJoinFileDATA4, ".\\DATA\\"), oldName);
        diffFile2 = open(gText, _O_BINARY);
        if (diffFile2 == -1)
            FileError(gText);
        read(diffFile2, oldData13, oldSize10);
        close(diffFile2);
        memcpy(joinData9, oldData13, oldSize10);

        position1 = JOIN_HEADER_SIZE;
        while (diffSize1 > position1) {
            copyFlag16 = diffData5[position1] >> DIFF_COPY_FLAG_SHIFT;
            copyLength9 = GetSkipCopyLen(diffData5, &position1);
            if (copyFlag16) {
                memcpy(joinData9 + joinSize37, diffData5 + position1, copyLength9);
                joinSize37 += copyLength9;
                position1 += copyLength9;
            } else {
                joinSize37 += copyLength9;
            }
        }
    }

    sprintf(gText, DATA_COMPGEN(0x004f7c88, createJoinFileSS5, "%s%s"), DATA_COMPGEN(0x004f7c80, createJoinFileDATA5, ".\\DATA\\"), joinName);
    joinFile0 = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (joinFile0 == -1)
        FileError(gText);
    write(joinFile0, joinData9, joinSize37);
    close(joinFile0);
    LogInt(
        const_cast<char*>(DATA_COMPGEN(0x004f7c90, createJoinFileNewJoinCRC, "New Join CRC")),
        calc_crc_long(joinData9, joinSize37),
        joinSize37,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );

    sprintf(gText, DATA_COMPGEN(0x004f7ca8, createJoinFileSS6, "%s%s"), DATA_COMPGEN(0x004f7ca0, createJoinFileDATA6, ".\\DATA\\"), oldName);
    joinFile0 = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (joinFile0 == -1)
        FileError(gText);
    write(joinFile0, joinData9, joinSize37);
    close(joinFile0);

    if (oldData13)
        H2_FREE_AT(oldData13, DATA_COMPGEN(0x004f7cb0, createJoinFileSourceFile4, RETAIL_FILE), createJoinFileSourceLineBase + 0x53);
    if (diffData5)
        H2_FREE_AT(diffData5, DATA_COMPGEN(0x004f7cd8, createJoinFileSourceFile5, RETAIL_FILE), createJoinFileSourceLineBase + 0x55);
    if (joinData9)
        H2_FREE_AT(joinData9, DATA_COMPGEN(0x004f7d00, createJoinFileSourceFile6, RETAIL_FILE), createJoinFileSourceLineBase + 0x57);
}

VA(0x004854d5, 0x5d)
i32 game::HeroIDToHeroPos(playerData* pd, i32 heroId) {
    i32 i;
    for (i = 0; i < pd->m_heroCount; i++) {
        if (pd->m_heroIds[i] == heroId)
            return i;
    }
    return -1;
}

VA(0x00485532, 0x5d)
i32 game::TownIDToTownPos(playerData* pd, i32 townId) {
    i32 i;
    for (i = 0; i < pd->m_townCount; i++) {
        if (pd->m_townIds[i] == townId)
            return i;
    }
    return -1;
}

VA(0x0048558f, 0x79f)
void game::SetupNewRumour(void) {
    char rumourBuffer6[RUMOUR_SCRATCH_CAPACITY];
    i32l categoryStats7[GAME_PLAYER_COUNT];
    i8 categoryOrder2[RUMOUR_CATEGORY_ORDER_CAPACITY];
    rumourEventExtra* event4;
    i32 eventIndex11;
    i32 attempts13;
    i32 roll2;
    i32 selectionRoll;
    i32 direction9;
    // Retail's event chances, category weights, and cDirections indices are
    // tavern-rumour selection payload.
    // NOLINTBEGIN(readability-magic-numbers)
    if (m_rumourEventCount != 0 && Random(0, 9) < static_cast<i32>(m_rumourEventCount)) {
        attempts13 = 0;
        while (attempts13++ < 200) {
            if (m_rumourEventCount > 1)
                eventIndex11 = Random(0, m_rumourEventCount - 1);
            else
                eventIndex11 = 0;
            event4 =
                reinterpret_cast<rumourEventExtra*>(ppMapExtra[m_rumourEventIndices[eventIndex11]]);
            if (strlen(event4->text) > 2 && event4->text[0] != '@') {
                strcpy(m_rumour, event4->text);
                event4->text[0] = '@';
                return;
            }
        }
    }

    if (Random(0, 100) < 30) {
        strcpy(m_rumour, cRandomTavernText[(giCurTurn / 7) % 8]);
    } else {
        roll2 = Random(0, 100);
        if (roll2 < 80 && giCurTurn > 1) {
            attempts13 = 0;
            while (attempts13++ < 200) {
                selectionRoll = Random(
                    IDX(THIEVES_CATEGORY_OBELISKS), IDX(THIEVES_CATEGORY_INCOME)
                );
                GetCategoryStats(
                    static_cast<TownThievesGuildCategory>(selectionRoll),
                    categoryStats7,
                    categoryOrder2
                );
                SortStats(categoryStats7, categoryOrder2);
                if (categoryStats7[1] != categoryStats7[0]) {
                    if (selectionRoll == IDX(THIEVES_CATEGORY_OBELISKS))
                        sprintf(
                            m_rumour,
                            DATA_COMPGEN(0x004f7d28, setupNewRumourSHasFoundTheMostObelisks, "%s has found the most obelisks."),
                            cPlayerNames[categoryOrder2[0]]
                        );
                    else if (selectionRoll == IDX(THIEVES_CATEGORY_ARTIFACTS))
                        sprintf(
                            m_rumour,
                            DATA_COMPGEN(0x004f7d48, setupNewRumourSHasFoundTheMostArtifacts, "%s has found the most artifacts."),
                            cPlayerNames[categoryOrder2[0]]
                        );
                    else if (selectionRoll == IDX(THIEVES_CATEGORY_ARMY_STRENGTH))
                        sprintf(
                            m_rumour,
                            DATA_COMPGEN(0x004f7d6c, setupNewRumourSHasTheMostPowerfulForces, "%s has the most powerful forces."),
                            cPlayerNames[categoryOrder2[0]]
                        );
                    else
                        sprintf(
                            m_rumour,
                            DATA_COMPGEN(0x004f7d90, setupNewRumourSEarnsTheMostGold, "%s earns the most gold."),
                            cPlayerNames[categoryOrder2[0]]
                        );
                    return;
                }
            }
            goto ultimateRumour;
        } else {
        ultimateRumour:
            selectionRoll = Random(0, 100);
            if (selectionRoll < 33) {
                if (!(IDX(m_mapHeader.width) * DATA_COMPGEN(0x004eb788, setupNewRumourConstant, 0.33) <= m_ultimateArtifactX
                      || IDX(m_mapHeader.height) * 0.33 <= m_ultimateArtifactX)) {
                    direction9 = 7;
                } else if (!(IDX(m_mapHeader.width) * 0.33 <= m_ultimateArtifactX
                             || m_ultimateArtifactX <= IDX(m_mapHeader.height) * DATA_COMPGEN(0x004eb790, setupNewRumourConstant2, 0.66))) {
                    direction9 = 5;
                } else if (!(IDX(m_mapHeader.width) * 0.33 <= m_ultimateArtifactX)) {
                    direction9 = 6;
                } else if (!(m_ultimateArtifactX <= IDX(m_mapHeader.width) * 0.66
                             || IDX(m_mapHeader.height) * 0.33 <= m_ultimateArtifactX)) {
                    direction9 = 1;
                } else if (!(m_ultimateArtifactX <= IDX(m_mapHeader.width) * 0.66
                             || m_ultimateArtifactX <= IDX(m_mapHeader.height) * 0.66)) {
                    direction9 = 3;
                } else if (!(m_ultimateArtifactX <= IDX(m_mapHeader.width) * 0.66)) {
                    direction9 = 2;
                } else if (!(IDX(m_mapHeader.height) * 0.33 <= m_ultimateArtifactX)) {
                    direction9 = 0;
                } else if (!(m_ultimateArtifactX <= IDX(m_mapHeader.height) * 0.66)) {
                    direction9 = 4;
                } else {
                    direction9 = 8;
                }
                sprintf(
                    m_rumour,
                    DATA_COMPGEN(0x004f7da8, setupNewRumourTheUltimateArtifactMayBeFound, "The ultimate artifact may be found in the %s regions of the world."),
                    cDirections[direction9]
                );
            } else if (selectionRoll < 66) {
                sprintf(
                    m_rumour,
                    DATA_COMPGEN(0x004f7dec, setupNewRumourTheUltimateArtifactMayBeFound2, "The ultimate artifact may be found %s."),
                    cRumourTerrainDescriptions
                        [IDX(giGroundToTerrain
                                 [gpAdvManager
                                      ->GetCell(m_ultimateArtifactX, m_ultimateArtifactY)
                                      ->m_terrainImageIndex])]
                );
            } else {
                sprintf(
                    m_rumour,
                    DATA_COMPGEN(0x004f7e14, setupNewRumourTheUltimateArtifactIsReallyThe, "The ultimate artifact is really the %s."),
                    gArtifactNames[IDX(m_ultimateArtifactId)]
                );
            }
        }
    }
    // NOLINTEND(readability-magic-numbers)
}

VA(0x00485d2e, 0xd9)
EventExtra* GetMapEvent(i32 x, i32 y) {
    i32 i;
    for (i = 0; i < gpGame->m_mapEventCount; i++) {
        EventExtra* ev = reinterpret_cast<EventExtra*>(ppMapExtra[gpGame->m_mapEventIndices[i]]);
        if (ev->x == x && ev->y == y && ev->active != 0
            && ev->players[PlayerEventByte(giCurPlayer)] != 0)
            return ev;
    }
    return NULL;
}

VA(0x00485e07, 0x34c)
void game::CheckForTimeEvent(void) {
    i32 dayNumber6;
    i32 eventIndex11;
    i32 resourceIndex27;
    timeEventExtra* event15;
    i32 primaryType14;
    i32 secondaryType18;
    i32 primaryAmount12;
    i32 secondaryAmount9;
    i32 resourceAmount7;

    dayNumber6 =
        (m_week - 1) * EVENT_DAYS_PER_WEEK + (m_month - 1) * EVENT_DAYS_PER_MONTH + m_day;
    for (eventIndex11 = 0; eventIndex11 < m_timeEventCount; eventIndex11++) {
        event15 = static_cast<timeEventExtra*>(ppMapExtra[m_timeEventIndices[eventIndex11]]);
        if (((gbHumanPlayer[giCurPlayer] && event15->appliesToHuman)
             || (!gbHumanPlayer[giCurPlayer] && event15->appliesToComputer))
            && event15->players[GetPlayerColor(static_cast<i8>(giCurPlayer))]
            && (event15->firstDay == dayNumber6
                || (event15->repeatInterval != 0 && event15->firstDay < dayNumber6
                    && (dayNumber6 - event15->firstDay) % event15->repeatInterval == 0))) {
            primaryType14 = -1;
            primaryAmount12 = 0;
            secondaryType18 = -1;
            secondaryAmount9 = 0;
            for (resourceIndex27 = 0; resourceIndex27 < EVENT_RESOURCE_COUNT;
                 resourceIndex27++) {
                resourceAmount7 = event15->resources[resourceIndex27];
                if (gpGame->m_players[giCurPlayer].m_resources[resourceIndex27]
                    < -resourceAmount7) {
                    resourceAmount7 = -gpGame->m_players[giCurPlayer].m_resources[resourceIndex27];
                }
                gpGame->m_players[giCurPlayer].m_resources[resourceIndex27] +=
                    event15->resources[resourceIndex27];
                if (gpGame->m_players[giCurPlayer].m_resources[resourceIndex27] < 0)
                    gpGame->m_players[giCurPlayer].m_resources[resourceIndex27] = 0;
                if (resourceAmount7 != 0) {
                    if (primaryType14 != -1) {
                        secondaryType18 = primaryType14;
                        secondaryAmount9 = primaryAmount12;
                    }
                    primaryType14 = resourceIndex27;
                    primaryAmount12 = resourceAmount7;
                }
            }
            if (primaryType14 >= 0 && primaryType14 <= EVENT_RESOURCE_COUNT - 1
                && primaryAmount12 < 0) {
                primaryAmount12 -= EVENT_RESOURCE_PENALTY;
            }
            if (secondaryType18 >= 0 && secondaryType18 <= EVENT_RESOURCE_COUNT - 1
                && secondaryAmount9 < 0) {
                secondaryAmount9 -= EVENT_RESOURCE_PENALTY;
            }
            if (gbThisNetHumanPlayer[giCurPlayer]) {
                NormalDialog(
                    event15->message,
                    1,
                    -1,
                    -1,
                    primaryType14,
                    primaryAmount12,
                    secondaryType18,
                    secondaryAmount9,
                    -1,
                    0
                );
            }
        }
    }
}

VA(0x00486153, 0x143)
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

VA(0x00486296, 0xab)
i32 CalcFileCRC(char* filename) {
    DATA(0x004f7e3c) static i16 calcFileCrcSourceLineBase = 0x1f5e;
    i32l size = FileSize(filename);
    char* block = static_cast<char*>(H2_ALLOC_AT(size, DATA_COMPGEN(0x004f7e40, calcFileCRCSourceFile, RETAIL_FILE), calcFileCrcSourceLineBase + 3));
    i32 hand = open(filename, _O_BINARY);
    if (hand == -1)
        FileError(filename);
    read(hand, block, size);
    i32 crc = calc_crc_long(reinterpret_cast<u8*>(block), size);
    close(hand);
    H2_FREE_AT(block, DATA_COMPGEN(0x004f7e68, calcFileCRCSourceFile2, RETAIL_FILE), calcFileCrcSourceLineBase + 14);
    return crc;
}

VA(0x00486341, 0x153)
void CompressTest2(void) {
    i32 dataSize2;
    i32l encodedSize14;
    i32l decodedSize17;
    char* sourceData6;
    i32 sourceCrc0;
    char* decodedData6;
    i32 index7;
    i32 decodedCrc5;
    i32 sourceCrcCheck7;
    char* encodedData6;

    dataSize2 = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
    sourceData6 =
        static_cast<char*>(
            H2_ALLOC_AT(dataSize2 + TEST_RANDOM_BUFFER_EXTRA, DATA_COMPGEN(0x004f7e94, compressTest2SourceFile, RETAIL_FILE), gCompressTest2SourceLine + 7)
        );
    encodedData6 =
        static_cast<char*>(
            H2_ALLOC_AT(dataSize2 + TEST_RANDOM_BUFFER_EXTRA, DATA_COMPGEN(0x004f7ebc, compressTest2SourceFile2, RETAIL_FILE), gCompressTest2SourceLine + 8)
        );
    decodedData6 =
        static_cast<char*>(
            H2_ALLOC_AT(dataSize2 + TEST_RANDOM_BUFFER_EXTRA, DATA_COMPGEN(0x004f7ee4, compressTest2SourceFile3, RETAIL_FILE), gCompressTest2SourceLine + 9)
        );
    for (index7 = 0; index7 < dataSize2; index7++)
        // Random() is inclusive, so this covers every byte payload value.
        sourceData6[index7] = static_cast<char>(Random(0, 255)); // NOLINT(readability-magic-numbers)
    sourceCrc0 = calc_crc_long(reinterpret_cast<u8*>(sourceData6), dataSize2);
    encodedSize14 = EncodeData(encodedData6, sourceData6, dataSize2);
    decodedSize17 = DecodeData(decodedData6, encodedData6, encodedSize14);
    decodedCrc5 = calc_crc_long(reinterpret_cast<u8*>(decodedData6), dataSize2);
    sourceCrcCheck7 = calc_crc_long(reinterpret_cast<u8*>(sourceData6), dataSize2);
    H2_FREE_AT(sourceData6, DATA_COMPGEN(0x004f7f0c, compressTest2SourceFile4, RETAIL_FILE), gCompressTest2SourceLine + 26);
    H2_FREE_AT(encodedData6, DATA_COMPGEN(0x004f7f34, compressTest2SourceFile5, RETAIL_FILE), gCompressTest2SourceLine + 27);
    H2_FREE_AT(decodedData6, DATA_COMPGEN(0x004f7f5c, compressTest2SourceFile6, RETAIL_FILE), gCompressTest2SourceLine + 28);
}

VA(0x00486494, 0x1be)
void CompressTest(void) {
    i32l fileSize7;
    i32l encodedSize14;
    i32l decodedSize17;
    char* sourceData6;
    i32 sourceCrc0;
    char* decodedData6;
    i32 fileHandle4;
    i32 decodedCrc5;
    i32 sourceCrcCheck7;
    char* encodedData6;
    char filename3[TEST_FILENAME_SIZE];

    LogStr(const_cast<char*>(DATA_COMPGEN(0x004f7f88, compressTestC1, "C1")));
    strcpy(filename3, DATA_COMPGEN(0x004f7f8c, compressTestCTEMPZDIF, "c:\\TEMP\\Z.DIF"));
    fileSize7 = FileSize(filename3);
    sourceData6 = static_cast<char*>(
        H2_ALLOC_AT(fileSize7 + TEST_FILE_BUFFER_EXTRA, DATA_COMPGEN(0x004f7f9c, compressTestSourceFile, RETAIL_FILE), gCompressTestSourceLine + 9)
    );
    encodedData6 = static_cast<char*>(
        H2_ALLOC_AT(fileSize7 + TEST_FILE_BUFFER_EXTRA, DATA_COMPGEN(0x004f7fc4, compressTestSourceFile2, RETAIL_FILE), gCompressTestSourceLine + 10)
    );
    decodedData6 = static_cast<char*>(
        H2_ALLOC_AT(fileSize7 + TEST_FILE_BUFFER_EXTRA, DATA_COMPGEN(0x004f7fec, compressTestSourceFile3, RETAIL_FILE), gCompressTestSourceLine + 11)
    );
    LogStr(const_cast<char*>(DATA_COMPGEN(0x004f8014, compressTestC2, "C2")));
    fileHandle4 = open(filename3, _O_BINARY);
    if (fileHandle4 == -1)
        FileError(filename3);
    read(fileHandle4, sourceData6, fileSize7);
    LogStr(const_cast<char*>(DATA_COMPGEN(0x004f8018, compressTestC3, "C3")));
    sourceCrc0 = calc_crc_long(reinterpret_cast<u8*>(sourceData6), fileSize7);
    LogStr(const_cast<char*>(DATA_COMPGEN(0x004f801c, compressTestC4, "C4")));
    close(fileHandle4);
    LogStr(const_cast<char*>(DATA_COMPGEN(0x004f8020, compressTestC5, "C5")));
    encodedSize14 = EncodeData(encodedData6, sourceData6, fileSize7);
    LogStr(const_cast<char*>(DATA_COMPGEN(0x004f8024, compressTestC6, "C6")));
    decodedSize17 = DecodeData(decodedData6, encodedData6, encodedSize14);
    LogStr(const_cast<char*>(DATA_COMPGEN(0x004f8028, compressTestC7, "C7")));
    decodedCrc5 = calc_crc_long(reinterpret_cast<u8*>(decodedData6), fileSize7);
    sourceCrcCheck7 = calc_crc_long(reinterpret_cast<u8*>(sourceData6), fileSize7);
    H2_FREE_AT(sourceData6, DATA_COMPGEN(0x004f802c, compressTestSourceFile4, RETAIL_FILE), gCompressTestSourceLine + 36);
    H2_FREE_AT(encodedData6, DATA_COMPGEN(0x004f8054, compressTestSourceFile5, RETAIL_FILE), gCompressTestSourceLine + 37);
    H2_FREE_AT(decodedData6, DATA_COMPGEN(0x004f807c, compressTestSourceFile6, RETAIL_FILE), gCompressTestSourceLine + 38);
    LogStr(const_cast<char*>(DATA_COMPGEN(0x004f80a4, compressTestC8, "C8")));
}

VA(0x00486652, 0x53)
void CompressTest3(void) {
    char buf[TEST_MESSAGE_CAPACITY];
    i32 i;
    for (i = 0; i < COMPRESS_TEST_ITERATIONS; i++) {
        sprintf(buf, DATA_COMPGEN(0x004f80a8, compressTest3TestD, "Test # %d"), i);
        AiPrint(buf);
        CompressTest2();
    }
}

VA(0x004866a5, 0x119)
i32 game::CountShrines(i32 player) {
    if (xIsExpansionMap == 0)
        return 0;
    i32 count = 0;
    mapCell* cell;
    {
        i32 col;
        i32 row;
        town* castle;
        for (row = 0; row < MAP_HEIGHT; row++) {
            for (col = 0; col < MAP_WIDTH; col++) {
                cell = WORLDMAP->Row(row) + col;
                if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
                    castle = GetCastle(cell->m_objectMetadata);
                    if (castle->m_owner == player
                        && (castle->m_buildings & IDX(TOWN_BUILDING_TAVERN))
                        && castle->m_type == FACTION_NECROMANCER)
                        count++;
                }
            }
        }
    }
    return count;
}

DATA(0x004f7550) i8 giMonType[] = {
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
DATA(0x004f7a08) char bMapInitialized = 0;
DATA(0x005280e8) i32 iViewArmyNumTroops;
DATA(0x005280ec) i8* gbNGHeroType;
DATA(0x005280f8) SMonFrameInfo sViewArmyMonFrameInfo;
DATA(0x00528430) i16 giUABaseX;
DATA(0x00528434) i16 giUABaseY;
DATA(0x00528438) i32 giEndSequence;
DATA(0x0052843c) b32 gbDismissArmy;
DATA(0x00528440) i8* gbNGHuman;
DATA(0x00528448) i32 iViewArmyFrame;
DATA(0x0052844c) b32 gbAllowUpgrade;
DATA(0x00528450) H2_ENUM_STORAGE(CreatureType, i32) iViewArmyType;
DATA(0x00528454) class hero* viewSpellsHero;
DATA(0x00528458) b32 gbUpgradeArmy;
DATA(0x00528460) i16 RandMineQty[AI_RANDOM_MINE_TYPE_COUNT];
DATA(0x00528470) char gcCurMapName[GAME_CURRENT_MAP_NAME_SIZE];
DATA(0x00528480) i8* gbNGDifficulty;
DATA(0x00528488) H2_ENUM_STORAGE(CreatureType, i32) iViewArmyUpgradeToType;
DATA(0x0052848c) i32 viewArmyBaseX;
DATA(0x00528490) i32 viewArmyBaseY;
DATA(0x00528498) i8* gbNGColor;
DATA(0x005284a0) i16 giUARadius;
DATA(0x005284a8) i8* gbNGPlayerPos;
DATA(0x005284b0) i32 viewArmyFacingWIPXMod;

#undef RETAIL_FILE
