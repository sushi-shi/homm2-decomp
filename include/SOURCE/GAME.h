#ifndef HOMM2_GAME_H
#define HOMM2_GAME_H

#include <va.h>
#include <SOURCE/KB_TYPES.h>

struct tag_message;

#pragma pack(push, 1)
struct EventExtra {
    u8 unknown00;
    i32 resources[7];
    i16 artifact;
    u8 applyToComputer;
    u8 cancelAfterVisit;
    char unknown21[4];
    i8 active;
    u16 x;
    u16 y;
    u8 unknown2a;
    u8 players[6];
    char message[1];
};
#pragma pack(pop)
SIZE(EventExtra, 0x32);
struct RandomHeroArmyRange {
    i16 creature;
    i16 minimum;
    i16 maximum;
};

H2_ENUM_BEGIN(GameSerializationConstant)
    GAME_SOURCE_LINE        = 660,
    GAME_LOAD_SOURCE_LINE   = 1103,
    GAME_SAVE_BUFFER_SIZE   = 50000,
    GAME_FILE_MARKER        = 1234,
    GAME_UNUSED_FILE_MARKER = 9999,
    GAME_PLAYER_COUNT       = 6,
    GAME_HERO_COUNT         = 54,
    GAME_TOWN_COUNT         = 72,
    GAME_MINE_COUNT         = 144,
    GAME_BOAT_COUNT         = 48
H2_ENUM_END(GameSerializationConstant)

// Action trigger ids carried in mapCell::m_triggerType (non-action id | the
// MAP_TRIGGER_ACTION_FLAG 0x80 bit). ProcessRandomObjects converts every
// MAP_TRIGGER_RANDOM_* cell into its concrete counterpart at map load.
// mapCell::m_objectMetadata layout for monster cells: low 12 bits carry the
// troop count, bit 0x1000 marks a placed guardian.
H2_ENUM_CLASS_BEGIN(GameMonsterMetadata)
    MAP_MONSTER_COUNT_MASK = 0xfff,
    MAP_MONSTER_GUARD_FLAG = 0x1000
H2_ENUM_CLASS_END(GameMonsterMetadata)

#define GAME_SCORE_EXTRA_LARGE_DAY_SCALE 0.6
#define GAME_SCORE_LARGE_DAY_SCALE 0.8
#define GAME_SCORE_SMALL_DAY_SCALE 1.4
#define GAME_VIEW_ARMY_FRAME_DELAY_SCALE 1.35

H2_ENUM_BEGIN(GameWeeklyConstant)
    WEEKLY_WATER_WHEEL_EMPTY        = 0xff,
    WEEKLY_MONSTER_POPULATION_LIMIT = 0x1fe1,
    WEEKLY_DWELLING_NO_GROWTH_FLAG  = 0x80,
    WEEKLY_FIRST_DWELLING           = 19,
    WEEKLY_LAST_DWELLING            = 30,
    WEEKLY_GROWTH_LIMIT             = 0x1feb,
    WEEKLY_DRAGON_CITY_LIMIT        = 220,
    WEEKLY_MONSTER_LIMIT            = 4000,
    WEEKLY_RECRUIT_TYPE_MASK        = 7,
    WEEKLY_RECRUIT_COUNT_SHIFT      = 3,
    WEEKLY_RECRUIT_MIN_GROWTH       = 2,
    WEEKLY_RECRUIT_MAX_GROWTH       = 5,
    WEEKLY_RECRUIT_LIMIT            = 1000,
    WEEKLY_SITE_TYPE_MASK           = 0x3f,
    WEEKLY_AVAILABLE_HERO           = 64
H2_ENUM_END(GameWeeklyConstant)

H2_ENUM_BEGIN(GameRandomTownConstant)
    NEUTRAL_TOWN_CASTLE_REINFORCE_CHANCE = 80,
    NEUTRAL_TOWN_REINFORCE_CHANCE        = 40,
    RANDOM_TOWN_UNOWNED_COLOR            = -1,
    RANDOM_TOWN_RACE_MIN                 = 0,
    RANDOM_TOWN_RACE_MAX                 = 5,
    RANDOM_TOWN_AGE                      = 10,
    RANDOM_TOWN_LEFT                     = -5,
    RANDOM_TOWN_TOP                      = -3,
    RANDOM_TOWN_RIGHT                    = 2,
    RANDOM_TOWN_BOTTOM                   = 1,
    RANDOM_TOWN_OBJECT_SOURCE_FIRST      = 0,
    RANDOM_TOWN_OBJECT_SOURCE_LAST       = 0x1f,
    RANDOM_TOWN_OVERLAY_SOURCE_FIRST     = 0x20,
    RANDOM_TOWN_OVERLAY_SOURCE_LAST      = 0xFF,
    RANDOM_TOWN_RACE_FRAME_SHIFT         = 5,
    RANDOM_TOWN_FIRST_TRIGGER            = 0x30,
    RANDOM_TOWN_SECOND_TRIGGER           = 0x31,
    RANDOM_TOWN_TRIGGER                  = 0x23
H2_ENUM_END(GameRandomTownConstant)

H2_ENUM_BEGIN(GameRandomHeroConstant)
    RANDOM_HERO_COUNT                     = 54,
    RANDOM_HERO_NORMAL_ARMY               = 0,
    RANDOM_HERO_EXPERIENCE_MIN            = 0,
    RANDOM_HERO_EXPERIENCE_MAX            = 50,
    RANDOM_HERO_EXPERIENCE_BASE           = 40,
    RANDOM_HERO_SEED_MIN                  = 1,
    RANDOM_HERO_SEED_MAX                  = 255,
    RANDOM_HERO_ENABLED                   = 1,
    RANDOM_HERO_STARTING_SPELL_KNOWN      = 1,
    RANDOM_HERO_STACK_SELECTED            = 1,
    RANDOM_HERO_FIRST_STACK_CHANCE        = 50,
    RANDOM_HERO_FIRST_STACK_BONUS_CHANCE  = 30,
    RANDOM_HERO_SECOND_STACK_CHANCE       = 25,
    RANDOM_HERO_SECOND_STACK_BONUS_CHANCE = 40,
    RANDOM_HERO_PERCENT_MIN               = 0,
    RANDOM_HERO_PERCENT_MAX               = 99,
    RANDOM_HERO_ARMY_SLOT_COUNT           = 5,
    RANDOM_HERO_ARMY_SELECTION_COUNT      = 2,
    RANDOM_HERO_ARMY_OPTION_COUNT         = 3,
    RANDOM_HERO_COUNT_SCALE               = 10,
    RANDOM_HERO_COUNT_ROUNDING            = 9,
    RANDOM_HERO_EMPTY_COUNT               = -1
H2_ENUM_END(GameRandomHeroConstant)

H2_ENUM_BEGIN(GameWaitConstant)
    WAIT_BOTTOM_VIEW_TIMEOUT = 9999999,
    WAIT_AMBIENT_MUSIC       = 21,
    WAIT_DIALOG_TYPE         = 9
H2_ENUM_END(GameWaitConstant)

H2_ENUM_BEGIN(GameHeroPoolConstant)
    AVAILABLE_HERO_SLOTS = 2
H2_ENUM_END(GameHeroPoolConstant)

i32 GetNumObelisks(i32 color);
void ComputeUALoc(i32);
void GenerateStandardFileName(char*, char*);
i32 ViewSpellsHandler(struct tag_message&);
i32 ViewSpecialHandler(struct tag_message&);
i32 ViewArmyHandler(struct tag_message&);
i32 IsCursedItem(ArtifactType item);
i32 CalcBaseScore(i32);
void WriteDiffHeaderInfo(u8 cmd, i32 len, u8* buf, i32* pos);
i32 GetSkipCopyLen(u8* buf, i32* pos);
void CreateDiffFile(char*, char*, char*, i32, i32);
void CreateJoinFile(char*, char*, char*);
EventExtra* GetMapEvent(i32 x, i32 y);
void CheckValidAvailableHeroes(void);
i32 CalcFileCRC(char* filename);
void CompressTest2(void);
void CompressTest(void);
void CompressTest3(void);

extern char bMapInitialized;

#endif
