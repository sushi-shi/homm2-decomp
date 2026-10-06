#ifndef HOMM2_EDITOR_HEROEDIT_H
#define HOMM2_EDITOR_HEROEDIT_H

// The hero editor (src/EDITOR/heroedit.cpp, heroedit.bin):
// eventsManager::EditHero, FillInHeroEdit and the dialog handler. A jailed
// hero's dialog offers the hero's class where a free hero's offers its
// patrol radius.

#include <va.h>
#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/ARMY.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/hero.h>

H2_ENUM_BEGIN(HeroEditConstant)
    // The map file's hero record and its unused tail.
    HERO_EDIT_RECORD_SIZE      = 0x4c,
    HERO_EDIT_RESERVED_SIZE    = 15,
    // heroedit.bin's controls.
    HERO_EDIT_STANDARD_ARMY    = 203,
    HERO_EDIT_STANDARD_ARMY_TOGGLE = 204,
    HERO_EDIT_CUSTOM_ARMY      = 205,
    HERO_EDIT_CUSTOM_ARMY_TOGGLE = 206,
    // The custom army's rows: shown with a custom army.
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
    HERO_EDIT_CLASS_LABEL      = 801,
    HERO_EDIT_PATROL           = 802,
    HERO_EDIT_PATROL_RADIUS    = 803,
    // The text fields' limits.
    HERO_EDIT_MAX_TROOP_COUNT  = 9999,
    HERO_EDIT_MAX_EXPERIENCE   = 999999,
    // The last portrait the arrows step to.
    HERO_EDIT_LAST_PORTRAIT    = 70,
    // A free hero patrols up to this many cells.
    HERO_EDIT_MAX_PATROL_RADIUS = 10,
    // The skill list: an entry per level and skill after "-empty-".
    HERO_EDIT_SKILL_LEVELS     = 3,
    // The heroes' classes a jailed hero can take.
    HERO_EDIT_CLASS_COUNT      = 6,
    HERO_EDIT_TEXT_SIZE        = 50,
    // The artifact lists leave out the editor-only artifacts from
    // ARTIFACT_EDITOR_ANY_ULTIMATE to ARTIFACT_SPELL_SCROLL.
    HERO_EDIT_HIDDEN_ARTIFACTS = 5,
    // SetWinText's row of the dialog.
    HERO_EDIT_TEXT_ROW         = 5
H2_ENUM_END(HeroEditConstant)

#pragma pack(push, 1)
// The map file's hero record as the editor edits it.
struct HeroExtra {
    i8 owner;
    u8 hasCustomArmy;
    H2_ENUM_STORAGE(CreatureType, i8) troopTypes[ARMY_GROUP_SLOT_COUNT];
    i16 troopCounts[ARMY_GROUP_SLOT_COUNT];
    u8 hasCustomPortrait;
    i8 portrait;
    i8 artifacts[EVENT_RECORD_HERO_ARTIFACT_COUNT];
    char unknown16;
    i32 experience;
    u8 hasCustomSkills;
    i8 skillTypes[HERO_SECONDARY_SKILL_CAPACITY];
    i8 skillLevels[HERO_SECONDARY_SKILL_CAPACITY];
    char unknown2c;
    u8 hasCustomName;
    char name[EVENT_RECORD_HERO_NAME_SIZE];
    u8 hasPatrol;
    // A free hero's patrol radius, or a jailed hero's class.
    i8 patrolRadius;
    char reserved3d[HERO_EDIT_RESERVED_SIZE];
};
#pragma pack(pop)
SIZE(HeroExtra, HERO_EDIT_RECORD_SIZE);

// The hero the open dialog edits, and whether it is jailed.
extern HeroExtra gHeroEdit;
extern b32 gEditJailedHero;

MessageDispatchResult HeroEditHandler(struct tag_message& message);

#endif
