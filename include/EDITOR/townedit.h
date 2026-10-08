#ifndef HOMM2_EDITOR_TOWNEDIT_H
#define HOMM2_EDITOR_TOWNEDIT_H

// The town editor (src/EDITOR/townedit.cpp, townedit.bin):
// eventsManager::EditTown, FillInTownEdit and the dialog handler.

#include <match.h>
#include <Domains.h>
#include <BASE/message.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/kbTypes.h>

H2_ENUM_BEGIN(TownEditConstant)
    // The map file's town record and its unused tail.
    TOWN_EDIT_RECORD_SIZE         = 0x46,
    TOWN_EDIT_RESERVED_SIZE       = 29,
    // townedit.bin's controls.
    TOWN_EDIT_STANDARD_ARMY       = 203,
    TOWN_EDIT_STANDARD_ARMY_TOGGLE = 204,
    TOWN_EDIT_CUSTOM_ARMY         = 205,
    TOWN_EDIT_CUSTOM_ARMY_TOGGLE  = 206,
    TOWN_EDIT_FIRST_ARMY_ROW      = 210,
    TOWN_EDIT_LAST_ARMY_ROW       = 239,
    TOWN_EDIT_FIRST_TROOP_TYPE    = 220,
    TOWN_EDIT_FIRST_TROOP_COUNT   = 230,
    TOWN_EDIT_CAPTAIN             = 302,
    // The castle rows: the castle allowance toggle and its labels.
    TOWN_EDIT_CASTLE_LABEL        = 310,
    TOWN_EDIT_CASTLE_TEXT         = 311,
    TOWN_EDIT_ALLOW_CASTLE        = 312,
    TOWN_EDIT_STANDARD_BUILDINGS  = 403,
    TOWN_EDIT_STANDARD_BUILDINGS_TOGGLE = 404,
    TOWN_EDIT_CUSTOM_BUILDINGS    = 405,
    TOWN_EDIT_CUSTOM_BUILDINGS_TOGGLE = 406,
    // The building rows: shown with custom buildings.
    TOWN_EDIT_FIRST_BUILDING_ROW  = 410,
    TOWN_EDIT_LAST_BUILDING_ROW   = 569,
    // A building's name label, toggle and the rows of a dwelling: the
    // label of building row i is i + 410, its toggle i + 450; a dwelling's
    // toggle is TOWN_EDIT_FIRST_DWELLING + TOWN_EDIT_DWELLING_STRIDE * level
    // and its upgrade's one more, with label rows TOWN_EDIT_LABEL_STEP and
    // twice that below.
    TOWN_EDIT_FIRST_BUILDING_NAME = 410,
    TOWN_EDIT_FIRST_BUILDING      = 450,
    // The tavern's toggle (gTownEditBuildings' second row), which a
    // necromancer town hides.
    TOWN_EDIT_TAVERN              = TOWN_EDIT_FIRST_BUILDING + 1,
    TOWN_EDIT_MAGE_GUILD          = 471,
    TOWN_EDIT_FIRST_DWELLING      = 550,
    TOWN_EDIT_LABEL_STEP          = 20,
    TOWN_EDIT_DWELLING_STRIDE     = 2,
    TOWN_EDIT_DWELLING_COUNT      = 6,
    TOWN_EDIT_BUILDING_COUNT      = 11,
    TOWN_EDIT_STANDARD_NAME       = 603,
    TOWN_EDIT_STANDARD_NAME_TOGGLE = 604,
    TOWN_EDIT_CUSTOM_NAME         = 605,
    TOWN_EDIT_CUSTOM_NAME_TOGGLE  = 606,
    TOWN_EDIT_NAME                = 607,
    // A troop count field's limit.
    TOWN_EDIT_MAX_TROOP_COUNT     = 9999
H2_ENUM_END(TownEditConstant)

#pragma pack(push, 1)
// The map file's town record as the editor edits it.
struct TownExtra {
    i8 owner;
    u8 hasCustomBuildings;
    u32 buildings;
    i8 mageGuildLevel;
    i8 hasCustomArmy;
    H2_ENUM_STORAGE(CreatureType, i8) troopTypes[ARMY_GROUP_SLOT_COUNT];
    i16 troopCounts[ARMY_GROUP_SLOT_COUNT];
    u8 hasCaptain;
    u8 hasCustomName;
    char name[EVENT_RECORD_TOWN_NAME_SIZE];
    H2_ENUM_STORAGE(FactionType, i8) faction;
    i8 isCastle;
    i8 disallowCastle;
    char reserved29[TOWN_EDIT_RESERVED_SIZE];
};
#pragma pack(pop)
SIZE(TownExtra, TOWN_EDIT_RECORD_SIZE);

// The town the open dialog edits, and the buildings of its building rows.
extern TownExtra gTownEdit;
extern H2_ENUM_STORAGE(BuildingSlotType, i32) gTownEditBuildings[TOWN_EDIT_BUILDING_COUNT];

MessageDispatchResult EditTownHandler(struct tag_message& message);

#endif // HOMM2_EDITOR_TOWNEDIT_H
