#ifndef HOMM2_SOURCE_GAMETYPES_H
#define HOMM2_SOURCE_GAMETYPES_H

#include <match.h>
#include <Domains.h>
#include <BASE/soundManager.h>
#include <SOURCE/kbTypes.h>

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
    GAME_HEROES_PER_FACTION      = GAME_HERO_COUNT / IDX(FACTION_COUNT),
    GAME_TOWN_COUNT              = 72,
    GAME_MINE_COUNT              = 144,
    GAME_BOAT_COUNT              = 48,
    GAME_EVENT_MESSAGE_HEAD_SIZE = 1
H2_ENUM_END(GameSerializationConstant)

H2_ENUM_BEGIN(GameCalendarConstant)
    CALENDAR_FIRST           = 1,
    CALENDAR_DAYS_PER_WEEK   = 7,
    CALENDAR_WEEKS_PER_MONTH = 4,
    CALENDAR_DAYS_PER_MONTH  = 28
H2_ENUM_END(GameCalendarConstant)

H2_ENUM_BEGIN(GameSetupSharedConstant)
    GAME_COMPUTER_PLAYER = 10
H2_ENUM_END(GameSetupSharedConstant)

#pragma pack(push, 1)
// A map event's or a time event's record: the editor's event dialog
// edits both. A map event has isMapEvent set and uses the artifact and the
// visit options; a time event uses the day fields and appliesToHuman.
struct EventExtra {
    u8 isMapEvent;
    i32 resources[IDX(RES_COUNT)];
    i16 artifact;
    u8 appliesToComputer;
    u8 cancelAfterVisit;
    u16 firstDay;
    u16 repeatInterval;
    b8 active;
    u16 x;
    u16 y;
    u8 appliesToHuman;
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
    MAP_MONSTER_FORCE_JOIN = 0x1000
H2_ENUM_CLASS_END(GameMonsterMetadata)

H2_ENUM_BEGIN(GameWeeklyConstant)
    WEEKLY_WATER_WHEEL_EMPTY        = 0xff,
    WEEKLY_MONSTER_POPULATION_LIMIT = 0x1fe1,
    WEEKLY_DWELLING_NO_GROWTH_FLAG  = 0x80,
    WEEKLY_GROWTH_LIMIT             = 0x1feb,
    WEEKLY_DRAGON_CITY_LIMIT        = 220,
    WEEKLY_MONSTER_LIMIT            = 4000,
    WEEKLY_RECRUIT_MIN_GROWTH       = 2,
    WEEKLY_RECRUIT_MAX_GROWTH       = 5,
    WEEKLY_RECRUIT_LIMIT            = 1000,
    WEEKLY_AVAILABLE_HERO           = 64
H2_ENUM_END(GameWeeklyConstant)

H2_ENUM_BEGIN(GameRandomTownConstant)
    RANDOM_TOWN_UNOWNED_COLOR        = -1,
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
    RANDOM_HERO_NORMAL_ARMY               = 0,
    RANDOM_HERO_EXPERIENCE_MIN            = 0,
    RANDOM_HERO_EXPERIENCE_MAX            = 50,
    RANDOM_HERO_EXPERIENCE_BASE           = 40,
    RANDOM_HERO_SEED_MIN                  = 1,
    RANDOM_HERO_SEED_MAX                  = 255,
    RANDOM_HERO_STARTING_SPELL_KNOWN      = 1,
    RANDOM_HERO_FIRST_STACK_CHANCE        = 50,
    RANDOM_HERO_FIRST_STACK_BONUS_CHANCE  = 30,
    RANDOM_HERO_SECOND_STACK_CHANCE       = 25,
    RANDOM_HERO_SECOND_STACK_BONUS_CHANCE = 40,
    RANDOM_HERO_PERCENT_MIN               = 0,
    RANDOM_HERO_PERCENT_MAX               = 99,
    RANDOM_HERO_ARMY_SELECTION_COUNT      = 2,
    RANDOM_HERO_ARMY_OPTION_COUNT         = 3,
    RANDOM_HERO_COUNT_SCALE               = 10,
    RANDOM_HERO_COUNT_ROUNDING            = 9,
    RANDOM_HERO_EMPTY_COUNT               = -1
H2_ENUM_END(GameRandomHeroConstant)

// spellwin.bin: game::ViewSpells' control ids and cSpellHelp rows (also CombatSpecialHandler's).
H2_ENUM_BEGIN(GameViewSpellsConstant)
    VIEW_SPELLS_WINDOW_X               = 86,
    VIEW_SPELLS_WINDOW_Y               = 87,
    VIEW_SPELL_PREVIOUS_ID             = 2,
    VIEW_SPELL_NEXT_ID                 = 3,
    VIEW_SPELL_ADVENTURE_TAB_ID        = 4,
    VIEW_SPELL_COMBAT_TAB_ID           = 5,
    VIEW_SPELL_MANA_LABEL_ID           = 6,
    VIEW_SPELL_MANA_HUNDREDS_ID        = 7,
    VIEW_SPELL_MANA_TENS_ID            = 8,
    VIEW_SPELL_MANA_ONES_ID            = 9,
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
    VIEW_SPELL_NAME_WIDTH              = 78,
    VIEW_SPELL_MANA_MAX                = 999,
    VIEW_SPELL_MANA_HUNDREDS_THRESHOLD = 99,
    VIEW_SPELL_MANA_TENS_THRESHOLD     = 9,
    VIEW_SPELL_MANA_HUNDREDS_DIVISOR   = 100,
    VIEW_SPELL_MANA_TENS_DIVISOR       = 10,
    VIEW_SPELL_MANA_DIGIT_BASE         = 10,
    VIEW_SPELL_HELP_PREVIOUS           = 0,
    VIEW_SPELL_HELP_NEXT               = 1,
    VIEW_SPELL_HELP_ADVENTURE          = 2,
    VIEW_SPELL_HELP_COMBAT             = 3,
    VIEW_SPELL_HELP_CLOSE              = 4,
    VIEW_SPELL_HELP_OTHER              = 5,
    VIEW_SPELL_HELP_SELECT_SPELL       = 6,
    VIEW_SPELL_HELP_COMBAT_DEFAULT     = 7,
    VIEW_SPELL_HELP_MANA               = 8
H2_ENUM_END(GameViewSpellsConstant)

H2_ENUM_BEGIN(GameWaitConstant)
    WAIT_BOTTOM_VIEW_TIMEOUT = 9999999,
    WAIT_AMBIENT_MUSIC       = MUSIC_TRACK_NEW_MONTH
H2_ENUM_END(GameWaitConstant)

#endif
