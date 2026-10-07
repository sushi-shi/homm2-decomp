#ifndef HOMM2_EDITOR_HEROEDIT_H
#define HOMM2_EDITOR_HEROEDIT_H


#include <Ints.h>
#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/ARMY.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/hero.h>

typedef enum HeroEditConstant {

    HERO_EDIT_RECORD_SIZE      = 0x4c,
    HERO_EDIT_RESERVED_SIZE    = 14,

    HERO_EDIT_STANDARD_ARMY    = 203,
    HERO_EDIT_STANDARD_ARMY_TOGGLE = 204,
    HERO_EDIT_CUSTOM_ARMY      = 205,
    HERO_EDIT_CUSTOM_ARMY_TOGGLE = 206,

    HERO_EDIT_FIRST_ARMY_ROW   = 210,
    HERO_EDIT_LAST_ARMY_ROW    = 239,
    HERO_EDIT_FIRST_TROOP_TYPE = 220,
    HERO_EDIT_FIRST_TROOP_COUNT = 230,
    HERO_EDIT_FIRST_ARTIFACT   = 308,
    HERO_EDIT_EXPERIENCE       = 401,
    HERO_EDIT_STANDARD_SKILLS  = 503,
    HERO_EDIT_STANDARD_SKILLS_TOGGLE = 504,
    HERO_EDIT_CUSTOM_SKILLS    = 505,
    HERO_EDIT_CUSTOM_SKILLS_TOGGLE = 506,
    HERO_EDIT_FIRST_SKILL_ROW  = 510,
    HERO_EDIT_LAST_SKILL_ROW   = 529,
    HERO_EDIT_FIRST_SKILL      = 520,
    HERO_EDIT_STANDARD_NAME    = 603,
    HERO_EDIT_STANDARD_NAME_TOGGLE = 604,
    HERO_EDIT_CUSTOM_NAME      = 605,
    HERO_EDIT_CUSTOM_NAME_TOGGLE = 606,
    HERO_EDIT_NAME             = 607,
    HERO_EDIT_PORTRAIT         = 702,
    HERO_EDIT_PREVIOUS_PORTRAIT = 703,
    HERO_EDIT_NEXT_PORTRAIT    = 704,
    HERO_EDIT_TYPE_LABEL       = 800,
    HERO_EDIT_PATROL_LABEL     = 801,
    HERO_EDIT_PATROL           = 802,
    HERO_EDIT_PATROL_RADIUS    = 803,

    HERO_EDIT_MAX_TROOP_COUNT  = 9999,
    HERO_EDIT_MAX_EXPERIENCE   = 999999,


    HERO_EDIT_LAST_PORTRAIT    = 70,
    HERO_EDIT_NO_PORTRAIT      = -1,

    HERO_EDIT_MAX_PATROL_RADIUS = 10,

    HERO_EDIT_SKILL_LEVELS     = H2EnumIndex(HERO_SKILL_LEVEL_COUNT) - H2EnumIndex(HERO_SKILL_LEVEL_BASIC)
} HeroEditConstant;

#pragma pack(push, 1)

struct HeroExtra {
    i8 owner;
    u8 hasCustomArmy;
    H2EnumStorage<CreatureType, i8> troopTypes[ARMY_GROUP_SLOT_COUNT];
    i16 troopCounts[ARMY_GROUP_SLOT_COUNT];
    u8 hasCustomPortrait;
    i8 portrait;
    i8 artifacts[EVENT_RECORD_HERO_ARTIFACT_SLOTS];
    i32 experience;
    u8 hasCustomSkills;
    i8 skillTypes[HERO_SECONDARY_SKILL_CAPACITY];
    i8 skillLevels[HERO_SECONDARY_SKILL_CAPACITY];
    char unknown2c;
    u8 hasCustomName;
    char name[EVENT_RECORD_HERO_NAME_SIZE];
    u8 hasPatrol;

    i8 patrolRadius;


    b8 hasAssignedHero;
    char reserved3e[HERO_EDIT_RESERVED_SIZE];
};
#pragma pack(pop)


extern HeroExtra gHeroEdit;
extern b32 gEditJailedHero;

MessageDispatchResult EditHeroHandler(struct tag_message& message);

#endif
