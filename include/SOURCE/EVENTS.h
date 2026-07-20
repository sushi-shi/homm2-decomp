#ifndef HOMM2_EVENTS_H
#define HOMM2_EVENTS_H

#include <Ints.h>
#include <SOURCE/KB_TYPES.h>

class hero;

H2_ENUM_BEGIN(EventConstant)
    COMBAT_REMOTE_BUFFER_SIZE             = 0xFF,
    COMBAT_REMOTE_HEADER_SIZE             = 0x9b,
    COMBAT_REMOTE_HERO_FIRST_SIZE         = 200,
    COMBAT_REMOTE_HERO_SECOND_SIZE        = 50,
    COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX = 52,
    COMBAT_REMOTE_PACKET_HEADER_SIZE      = 9,
    COMBAT_REMOTE_FRAGMENT_TYPE           = 1,
    COMBAT_REMOTE_TIMEOUT                 = 90000
H2_ENUM_END(EventConstant)

H2_ENUM_BEGIN(EventRecordConstant)
    EVENT_RECORD_RESOURCE_COUNT          = IDX(RES_COUNT),
    EVENT_RECORD_PLAYER_COUNT            = 6,
    EVENT_RECORD_ARMY_SLOT_COUNT         = 5,
    EVENT_RECORD_SKILL_CAPACITY          = 8,
    EVENT_RECORD_MAP_ANSWER_COUNT        = 8,
    EVENT_RECORD_MAP_ANSWER_SIZE         = 13,
    EVENT_RECORD_VARIABLE_TEXT_HEAD_SIZE = 1,
    EVENT_RECORD_SIGN_HEADER_SIZE        = 9,
    EVENT_RECORD_RUMOUR_HEADER_SIZE      = 8,
    EVENT_RECORD_TIME_GAP_FIRST_SIZE     = 2,
    EVENT_RECORD_TIME_GAP_SECOND_SIZE    = 5,
    EVENT_RECORD_HERO_ARTIFACT_COUNT     = 3,
    EVENT_RECORD_HERO_NAME_SIZE          = 13,
    EVENT_RECORD_TOWN_NAME_SIZE          = 15
H2_ENUM_END(EventRecordConstant)

#pragma pack(push, 1)
struct mapEventExtra {
    u8 active;
    i32 resources[EVENT_RECORD_RESOURCE_COUNT];
    i16 artifact;
    u8 answerCount;
    char answers[EVENT_RECORD_MAP_ANSWER_COUNT][EVENT_RECORD_MAP_ANSWER_SIZE];
    char riddle[EVENT_RECORD_VARIABLE_TEXT_HEAD_SIZE];
};
struct signEventExtra {
    char pad[EVENT_RECORD_SIGN_HEADER_SIZE];
    char text[EVENT_RECORD_VARIABLE_TEXT_HEAD_SIZE];
};
struct rumourEventExtra {
    char pad[EVENT_RECORD_RUMOUR_HEADER_SIZE];
    char text[EVENT_RECORD_VARIABLE_TEXT_HEAD_SIZE];
};
struct timeEventExtra {
    char unknown00;
    i32 resources[EVENT_RECORD_RESOURCE_COUNT];
    char unknown1d[EVENT_RECORD_TIME_GAP_FIRST_SIZE];
    u8 appliesToComputer;
    char unknown20;
    u16 firstDay;
    u16 repeatInterval;
    char unknown25[EVENT_RECORD_TIME_GAP_SECOND_SIZE];
    u8 appliesToHuman;
    u8 players[EVENT_RECORD_PLAYER_COUNT];
    char message[EVENT_RECORD_VARIABLE_TEXT_HEAD_SIZE];
};
struct mapHeroExtra {
    i8 owner;
    u8 hasCustomArmy;
    i8 troopTypes[EVENT_RECORD_ARMY_SLOT_COUNT];
    u16 troopCounts[EVENT_RECORD_ARMY_SLOT_COUNT];
    u8 hasCustomHero;
    i8 heroId;
    i8 artifacts[EVENT_RECORD_HERO_ARTIFACT_COUNT];
    char unknown16;
    i32 experience;
    u8 hasCustomSkills;
    i8 skillTypes[EVENT_RECORD_SKILL_CAPACITY];
    i8 skillLevels[EVENT_RECORD_SKILL_CAPACITY];
    char unknown2c;
    u8 hasCustomName;
    char name[EVENT_RECORD_HERO_NAME_SIZE];
    u8 hasPatrol;
    union {
        i8 patrolRadius;
        i8 heroClass;
    };
    i8 hasAssignedHero;
};
struct mapTownExtra {
    i8 color;
    u8 hasCustomBuildings;
    u32 buildings;
    i8 mageGuildLevel;
    i8 hasCustomArmy;
    i8 troopTypes[EVENT_RECORD_ARMY_SLOT_COUNT];
    u16 troopCounts[EVENT_RECORD_ARMY_SLOT_COUNT];
    u8 hasShrine;
    char unused18;
    char name[EVENT_RECORD_TOWN_NAME_SIZE];
    i8 unknown28;
};
#pragma pack(pop)

H2_ENUM_BEGIN(MapEventConstant)
    MAP_EVENT_DATA_EMPTY                 = 0,
    MAP_EVENT_ARTIFACT_GUARD_FLAG        = 0x100,
    MAP_EVENT_ARTIFACT_CONDITION_FLAG    = 0x80,
    MAP_EVENT_ARTIFACT_CONDITION_ID_MASK = 0x7f,
    MAP_EVENT_ARTIFACT_ID_MASK           = 0xff,
    MAP_EVENT_DATA_SHIFT                 = 3,
    MAP_EVENT_RESOURCE_OFFSET            = 1,
    MAP_EVENT_RESOURCE_AMOUNT            = 5,
    MAP_EVENT_GOLD_AMOUNT                = 500,
    CAMPFIRE_RESOURCE_MASK               = 0xf,
    CAMPFIRE_AMOUNT_SHIFT                = 4,
    CAMPFIRE_GOLD_MULTIPLIER             = 100,
    OASIS_MOBILITY_BONUS                 = 800,
    EVENT_ARTIFACT_CAPACITY              = 14,
    SKELETON_EMPTY                       = 1,
    SKELETON_ARTIFACT_OFFSET             = 2,
    SKELETON_GOLD                        = 1000,
    CHEST_ARTIFACT_FLAG                  = 0x100,
    CHEST_ARTIFACT_MASK                  = 0xFF,
    CHEST_GOLD_ONLY                      = 2,
    CHEST_GOLD_MULTIPLIER                = 500,
    CHEST_EXPERIENCE_MULTIPLIER          = 125,
    DAEMON_CAVE_EMPTY                    = 1,
    DAEMON_REWARD_EXPERIENCE             = 2,
    DAEMON_REWARD_ARTIFACT               = 3,
    DAEMON_REWARD_EXPERIENCE_GOLD        = 4,
    DAEMON_REWARD_RANSOM                 = 5,
    DAEMON_SERVANT_BASE                  = 0x3e,
    DAEMON_SERVANT_COUNT                 = 8,
    DAEMON_REWARD_MASK                   = 0xf,
    DAEMON_SERVANT_MASK                  = 0xf0,
    DAEMON_SERVANT_SHIFT                 = 4,
    DAEMON_EXPERIENCE                    = 1000,
    DAEMON_GOLD                          = 2500,
    EVENT_RANDOM_ARTIFACT_GOLD           = 1000,
    GAZEBO_EXPERIENCE                    = 1000,
    MINE_GOLD_INCOME                     = 1000,
    MINE_ORE_INCOME                      = 2,
    MINE_RESOURCE_ICON_OFFSET            = 59,
    WINDMILL_RESOURCE_AMOUNT             = 2,
    WINDMILL_EMPTY                       = 99,
    SEA_CHEST_GOLD                       = 1500,
    SEA_CHEST_ARTIFACT_GOLD              = 1000,
    MAGELLAN_MAP_COST                    = 1000,
    WATERING_HOLE_MOBILITY_BONUS         = 400,
    HERO_SECONDARY_SKILL_LIMIT           = 8,
    XANADU_ADMISSION_LEVEL               = 10,
    TREE_KNOWLEDGE_MODE_SHIFT            = 6,
    TREE_KNOWLEDGE_FREE                  = 1,
    TREE_KNOWLEDGE_GOLD                  = 2,
    TREE_KNOWLEDGE_GEMS                  = 3,
    TREE_KNOWLEDGE_GOLD_COST             = 2000,
    TREE_KNOWLEDGE_GEM_COST              = 10,
    DWELLING_GUARDED_FLAG                = 0x100,
    PYRAMID_GUARD_COUNT                  = 50,
    PYRAMID_WISDOM_REQUIRED              = 3,
    SHRINE_WISDOM_BONUS                  = 2,
    EVENT_TEXT_BUFFER_SIZE               = 500,
    EVENT_TEXT_ALCHEMIST_CAPTURED        = 0,
    EVENT_TEXT_SIGNPOST                  = 1,
    EVENT_TEXT_BUOY_VISITED              = 2,
    EVENT_TEXT_BUOY_REWARD               = 3,
    EVENT_TEXT_FAERIE_RING_VISITED       = 12,
    EVENT_TEXT_FAERIE_RING_REWARD        = 13,
    EVENT_TEXT_CAMPFIRE                  = 14,
    EVENT_TEXT_FOUNTAIN_VISITED          = 15,
    EVENT_TEXT_FOUNTAIN_REWARD           = 16,
    EVENT_TEXT_GAZEBO_VISITED            = 17,
    EVENT_TEXT_GAZEBO_REWARD             = 18,
    EVENT_TEXT_GENIE_LAMP                = 19,
    EVENT_TEXT_GRAVEYARD_PROMPT          = 20,
    EVENT_TEXT_GRAVEYARD_EMPTY           = 21,
    EVENT_TEXT_GRAVEYARD_REWARD          = 22,
    EVENT_TEXT_LIGHTHOUSE_CAPTURED       = 58,
    EVENT_TEXT_WATER_WHEEL_EMPTY         = 59,
    EVENT_TEXT_WATER_WHEEL_REWARD        = 60,
    EVENT_TEXT_FOLLOWERS                 = 66,
    EVENT_TEXT_MONSTER_REFUSAL           = 67,
    EVENT_TEXT_OBELISK_REWARD            = 68,
    EVENT_TEXT_OBELISK_VISITED           = 69,
    EVENT_TEXT_RESOURCE_PICKUP           = 72,
    EVENT_TEXT_SAWMILL_CAPTURED          = 73,
    EVENT_TEXT_ORACLE                    = 74,
    EVENT_TEXT_DESERT_TENT_EMPTY         = 81,
    EVENT_TEXT_DESERT_TENT_RECRUIT       = 82,
    EVENT_TEXT_WAGON_EMPTY               = 83,
    EVENT_TEXT_WAGON_RECRUIT             = 84,
    EVENT_TEXT_WHIRLPOOL                 = 85,
    EVENT_TEXT_WINDMILL_EMPTY            = 86,
    EVENT_TEXT_WINDMILL_REWARD           = 87,
    EVENT_TEXT_SKELETON_EMPTY            = 93,
    EVENT_TEXT_SKELETON_REWARD           = 94,
    EVENT_TEXT_COUNT                     = 95,
    WAGON_ARTIFACT_FLAG                  = 0x80,
    WAGON_ARTIFACT_MASK                  = 0x7f,
    ARTIFACT_EVENT_GUARDED_FLAG          = 0x100,
    ARTIFACT_EVENT_MONSTER_MASK          = 0x7f,
    ARTIFACT_EVENT_MODE_MASK             = 0xf,
    ARTIFACT_EVENT_RESOURCE_MASK         = 0xf0,
    ARTIFACT_EVENT_RESOURCE_SHIFT        = 4,
    ARTIFACT_EVENT_MODE_PICKUP           = 1,
    ARTIFACT_EVENT_MODE_GOLD             = 3,
    ARTIFACT_EVENT_MODE_WISDOM           = 4,
    ARTIFACT_EVENT_MODE_LEADERSHIP       = 5,
    ARTIFACT_EVENT_MODE_RESOURCE_3       = 6,
    ARTIFACT_EVENT_MODE_RESOURCE_5       = 7,
    ARTIFACT_EVENT_GOLD_COST             = 2000,
    ARTIFACT_EVENT_RESOURCE_3_GOLD_COST  = 2500,
    ARTIFACT_EVENT_RESOURCE_5_GOLD_COST  = 3000,
    ARTIFACT_EVENT_RESOURCE_3_AMOUNT     = 3,
    ARTIFACT_EVENT_RESOURCE_5_AMOUNT     = 5,
    ARTIFACT_EVENT_GUARD_ROGUE_COUNT     = 50,
    ABANDONED_MINE_X_MIN                 = -2,
    ABANDONED_MINE_X_MAX                 = 1,
    ABANDONED_MINE_Y_TOP                 = -1,
    ABANDONED_MINE_EVENT                 = 23,
    ABANDONED_MINE_OBJECT                = 64,
    MAP_EVENT_REWARD_NONE                = -1,
    MAP_EVENT_REWARD_ARTIFACT            = 7,
    SPHINX_ANSWER_BUFFER_SIZE            = 52,
    SPHINX_INPUT_LENGTH                  = 12,
    SPHINX_RESOURCE_COUNT                = 7,
    OBSERVATION_TOWER_RADIUS             = 20,
    ORACLE_WINDOW_TEXT_ID                = 14,
    ORACLE_THIEVES_GUILD_RANK            = 99,
    TELEPORT_TRIGGER_FLAG                = 0x80,
    STONE_LITHS_MIN_DISTANCE             = 1,
    WHIRLPOOL_MIN_DISTANCE               = 3,
    HERO_TOWN_LOCATION                   = 0xa3,
    BOAT_RESTORE_MODE                    = 2,
    COAST_FIZZLE_X                       = 192,
    COAST_FIZZLE_Y                       = 192,
    COAST_FIZZLE_WIDTH                   = 96,
    COAST_FIZZLE_HEIGHT                  = 96
H2_ENUM_END(MapEventConstant)

H2_ENUM_BEGIN(FlotsamReward)
    FLOTSAM_EMPTY          = 0,
    FLOTSAM_WOOD           = 1,
    FLOTSAM_WOOD_AND_GOLD  = 2,
    FLOTSAM_LARGE_TREASURE = 3
H2_ENUM_END(FlotsamReward)

#define GENERIC_SITE_SIREN_ARMY_REMAINDER 0.7

H2_ENUM_BEGIN(MonsterInteractionConstant)
    MONSTER_JOIN_FORCED                     = 0x1000,
    MONSTER_COUNT_MASK                      = 0xfff,
    MONSTER_DIPLOMACY_NONE                  = 0,
    MONSTER_DIPLOMACY_BASIC                 = 1,
    MONSTER_DIPLOMACY_ADVANCED              = 2,
    MONSTER_DIPLOMACY_EXPERT                = 3,
    MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR = 2,
    MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR    = 4,
    MONSTER_DIALOG_YES                      = 0x7805,
    MONSTER_DIALOG_NO                       = 0x7806,
    MONSTER_OFFER_BUFFER_SIZE               = 300
H2_ENUM_END(MonsterInteractionConstant)

#define MONSTER_STRENGTH_JOIN 2.0
#define MONSTER_STRENGTH_FLEE 5.0

#define MONSTER_NECROMANCY_FRACTION 0.1
#define MONSTER_AI_JOIN_COST_FRACTION 0.75

H2_ENUM_BEGIN(EventArtifactStatConstant)
    EVENT_ARTIFACT_PRIMARY_STAT_COUNT     = 4,
    EVENT_ARTIFACT_SLOT_COUNT             = 14,
    EVENT_ARTIFACT_NON_TRANSFERABLE_LAST  = IDX(ARTIFACT_ULTIMATE_WAND),
    EVENT_ARTIFACT_TAKE                   = 1,
    EVENT_ARTIFACT_SPELL_POINT_MULTIPLIER = 10
H2_ENUM_END(EventArtifactStatConstant)

H2_ENUM_BEGIN(EventEffectConstant)
    EVENT_WHIRLPOOL_TRIGGER_ROLL     = 1,
    EVENT_WHIRLPOOL_TRIGGER_MAX      = 3,
    EVENT_WHIRLPOOL_ARMY_VALUE_LIMIT = 99999999,
    EVENT_FIZZLE_HERO_LOSS           = 0,
    EVENT_FIZZLE_ARTIFACT            = 1,
    EVENT_FIZZLE_X                   = 168,
    EVENT_FIZZLE_Y                   = 160,
    EVENT_FIZZLE_WIDTH               = 132,
    EVENT_FIZZLE_HEIGHT              = 132,
    EVENT_FIZZLE_STEPS               = 65
H2_ENUM_END(EventEffectConstant)

i8 StrEqNoCase(char*, char*);
i32 GiveArtifact(class hero*, ArtifactType, b32, i8);
void GiveTakeArtifactStat(class hero*, ArtifactType, b32);
i32 RiddleStringsEqual(char*, char*);

#endif
