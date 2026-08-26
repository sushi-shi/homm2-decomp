#ifndef HOMM2_GAME_H
#define HOMM2_GAME_H

#include <va.h>
#include <BASE/message.h>
#include <SOURCE/KB_TYPES.h>

struct tag_message;

H2_ENUM_BEGIN(PlayerColor)
    PLAYER_COLOR_BLUE   = 0,
    PLAYER_COLOR_GREEN  = 1,
    PLAYER_COLOR_RED    = 2,
    PLAYER_COLOR_YELLOW = 3,
    PLAYER_COLOR_ORANGE = 4,
    PLAYER_COLOR_PURPLE = 5,
    PLAYER_COLOR_COUNT  = 6
H2_ENUM_END(PlayerColor)

H2_ENUM_CLASS_BEGIN_SPLIT(GameDifficulty, i8)
    DIFFICULTY_EASY       = 0,
    DIFFICULTY_NORMAL     = 1,
    DIFFICULTY_HARD       = 2,
    DIFFICULTY_EXPERT     = 3,
    DIFFICULTY_IMPOSSIBLE = 4,
    DIFFICULTY_COUNT      = 5
H2_ENUM_CLASS_END_SPLIT(GameDifficulty, i8)

H2_ENUM_BEGIN(GameSerializationConstant)
    GAME_SOURCE_LINE             = 660,
    GAME_LOAD_SOURCE_LINE        = 1103,
    GAME_SAVE_BUFFER_SIZE        = 50000,
    GAME_FILE_MARKER             = 1234,
    GAME_UNUSED_FILE_MARKER      = 9999,
    GAME_PLAYER_COUNT            = PLAYER_COLOR_COUNT,
    GAME_HERO_COUNT              = 54,
    GAME_TOWN_COUNT              = 72,
    GAME_MINE_COUNT              = 144,
    GAME_BOAT_COUNT              = 48,
    GAME_EVENT_RUNTIME_GAP_SIZE  = 4,
    GAME_EVENT_MESSAGE_HEAD_SIZE = 1
H2_ENUM_END(GameSerializationConstant)

H2_ENUM_BEGIN(GameCalendarConstant)
    CALENDAR_DAYS_PER_WEEK  = 7,
    CALENDAR_DAYS_PER_MONTH = 28
H2_ENUM_END(GameCalendarConstant)

H2_ENUM_BEGIN(GameSetupSharedConstant)
    GAME_COMPUTER_PLAYER = 10
H2_ENUM_END(GameSetupSharedConstant)

#pragma pack(push, 1)
struct EventExtra {
    u8 unknown00;
    i32 resources[IDX(RES_COUNT)];
    i16 artifact;
    u8 applyToComputer;
    u8 cancelAfterVisit;
    char unknown21[GAME_EVENT_RUNTIME_GAP_SIZE];
    b8 active;
    u16 x;
    u16 y;
    u8 unknown2a;
    u8 players[GAME_PLAYER_COUNT];
    char message[GAME_EVENT_MESSAGE_HEAD_SIZE];
};
#pragma pack(pop)
SIZE(EventExtra, 0x32);
struct RandomHeroArmyRange {
    i16 creature;
    i16 minimum;
    i16 maximum;
};

H2_ENUM_CLASS_BEGIN(GameMonsterMetadata)
    MAP_MONSTER_COUNT_MASK = 0xfff,
    MAP_MONSTER_GUARD_FLAG = 0x1000
H2_ENUM_CLASS_END(GameMonsterMetadata)

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
    RANDOM_TOWN_UNOWNED_COLOR        = -1,
    RANDOM_TOWN_RACE_MIN             = 0,
    RANDOM_TOWN_RACE_MAX             = 5,
    RANDOM_TOWN_AGE                  = 10,
    RANDOM_TOWN_LEFT                 = -5,
    RANDOM_TOWN_TOP                  = -3,
    RANDOM_TOWN_RIGHT                = 2,
    RANDOM_TOWN_BOTTOM               = 1,
    RANDOM_TOWN_OBJECT_SOURCE_FIRST  = 0,
    RANDOM_TOWN_OBJECT_SOURCE_LAST   = 0x1f,
    RANDOM_TOWN_OVERLAY_SOURCE_FIRST = 0x20,
    RANDOM_TOWN_OVERLAY_SOURCE_LAST  = 0xFF,
    RANDOM_TOWN_RACE_FRAME_SHIFT     = 5
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
MessageDispatchResult ViewSpellsHandler(struct tag_message&);
MessageDispatchResult ViewSpecialHandler(struct tag_message&);
MessageDispatchResult ViewArmyHandler(struct tag_message&);
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

extern bchar bMapInitialized;

#endif
