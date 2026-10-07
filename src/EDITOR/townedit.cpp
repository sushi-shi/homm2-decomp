// The town editor (townedit.bin): eventsManager::EditTown, the dialog's
// fill-in and its handler. The unit name is descriptive: the dialog
// resource names it.

#include <va.h>
#include <EDITOR/townedit.h>
#include <EDITOR/eventsManager.h>
#include <EDITOR/editManager.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/mapcell.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/message.h>
#include <BASE/widget.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DATA(0x00499714)
H2_ENUM_STORAGE(BuildingSlotType, i32) gTownEditBuildings[TOWN_EDIT_BUILDING_COUNT] = {
    BUILDING_SLOT_THIEVES_GUILD,
    BUILDING_SLOT_TAVERN,
    BUILDING_SLOT_DOCK,
    BUILDING_SLOT_WELL,
    TOWN_OBJECT_STATUE,
    TOWN_OBJECT_LEFT_TURRET,
    TOWN_OBJECT_RIGHT_TURRET,
    TOWN_OBJECT_MARKETPLACE,
    BUILDING_SLOT_WELL_EXTRA,
    TOWN_OBJECT_MOAT,
    BUILDING_SLOT_SPECIAL
};

// Zero-initialized: .bss in definition order.
DATA(0x004a5868) TownExtra gTownEdit = {0};

VA(0x00429e80, 0x4cc)
void eventsManager::EditTown(i32 x, i32 y) {
    // Never read: a copy of the cell and a slot of the retail frame.
    mapCell original;
    i32 H2_UNUSED(unused);
    i32 slot;
    i32 j;
    tag_message msg;

    gEditCell = gMap.GetCell(x, y);
    if (!gEditCell->m_objectMetadata) {
        NormalDialog(localization::Tr("editor.events.town.old_editor"), NORMAL_DIALOG_INFO);
        return;
    }
    original = *gEditCell;
    memcpy(&gTownEdit, gEditManager->m_extras[gEditCell->m_objectMetadata], sizeof(TownExtra));
    gEditDialog = new heroWindow(0, 0, "townedit.bin");
    SetWinText(gEditDialog, EDITOR_WIN_TEXT_TOWN);
    msg.type = MESSAGE_WIDGET;
    msg.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; slot++) {
        sprintf(gText, localization::Tr("editor.events.empty_entry"));
        msg.payload.widget.data.text = gText;
        msg.payload.widget.id = slot + TOWN_EDIT_FIRST_TROOP_TYPE;
        gEditDialog->BroadcastMessage(msg);
        for (j = 0; j < IDX(CREATURE_COUNT); j++) {
            sprintf(gText, "%s", gArmyNames[j]);
            gText[0] = CyrillicToUpper(gText[0]);
            msg.payload.widget.data.text = gText;
            msg.payload.widget.id = slot + TOWN_EDIT_FIRST_TROOP_TYPE;
            gEditDialog->BroadcastMessage(msg);
        }
    }
    msg.payload.widget.data.text = gText;
    msg.payload.widget.id = TOWN_EDIT_MAGE_GUILD;
    sprintf(gText, "%s", localization::Tr("editor.events.town.mage_guild.none"));
    gEditDialog->BroadcastMessage(msg);
    sprintf(gText, "%s", localization::Tr("editor.events.town.mage_guild.level_1"));
    gEditDialog->BroadcastMessage(msg);
    sprintf(gText, "%s", localization::Tr("editor.events.town.mage_guild.level_2"));
    gEditDialog->BroadcastMessage(msg);
    sprintf(gText, "%s", localization::Tr("editor.events.town.mage_guild.level_3"));
    gEditDialog->BroadcastMessage(msg);
    sprintf(gText, "%s", localization::Tr("editor.events.town.mage_guild.level_4"));
    gEditDialog->BroadcastMessage(msg);
    sprintf(gText, "%s", localization::Tr("editor.events.town.mage_guild.level_5"));
    gEditDialog->BroadcastMessage(msg);
    for (slot = 0; slot < TOWN_EDIT_BUILDING_COUNT; slot++) {
        msg.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        msg.payload.widget.id = slot + TOWN_EDIT_FIRST_BUILDING_NAME;
        msg.payload.widget.data.text = gText;
        if (gTownEditBuildings[slot] == BUILDING_SLOT_SPECIAL)
            sprintf(gText, gSpecialBuildingNames[IDX(gTownEdit.faction)]);
        else if (gTownEditBuildings[slot] == BUILDING_SLOT_WELL_EXTRA)
            sprintf(gText, gWellExtraNames[IDX(gTownEdit.faction)]);
        else
            sprintf(gText, gNeutralBuildingNames[IDX(gTownEditBuildings[slot])]);
        gEditDialog->BroadcastMessage(msg);
    }
    FillInTownEdit(&gTownEdit);
    gpWindowManager->DoDialog(gEditDialog, EditTownHandler, 0);
    delete gEditDialog;
    if (gpWindowManager->m_dialogResult != EVENTS_DIALOG_CANCEL) {
        gEditManager->m_mapChanged = true;
        memcpy(gEditManager->m_extras[gEditCell->m_objectMetadata], &gTownEdit, sizeof(TownExtra));
    }
    gEditManager->DrawMap();
    gEditManager->UpdateMapView();
}

VA(0x0042a34c, 0x5e4)
void eventsManager::FillInTownEdit(TownExtra* town) {
    char text[EVENTS_FIELD_TEXT_SIZE];
    i32 i;
    i32 index;
    tag_message msg;

    msg.type = MESSAGE_WIDGET;
    msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
    msg.payload.widget.id = TOWN_EDIT_CUSTOM_ARMY_TOGGLE;
    msg.payload.widget.command =
        town->hasCustomArmy ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    gEditDialog->BroadcastMessage(msg);
    msg.payload.widget.id = TOWN_EDIT_STANDARD_ARMY_TOGGLE;
    msg.payload.widget.command =
        town->hasCustomArmy ? WIDGET_COMMAND_CLEAR_FLAGS : WIDGET_COMMAND_SET_FLAGS;
    gEditDialog->BroadcastMessage(msg);
    for (i = TOWN_EDIT_FIRST_ARMY_ROW; i <= TOWN_EDIT_LAST_ARMY_ROW; i++) {
        msg.payload.widget.id = i;
        msg.payload.widget.command =
            town->hasCustomArmy ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
        gEditDialog->BroadcastMessage(msg);
    }
    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        sprintf(text, "%d", town->troopCounts[i]);
        msg.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        msg.payload.widget.data.text = text;
        msg.payload.widget.id = i + TOWN_EDIT_FIRST_TROOP_COUNT;
        gEditDialog->BroadcastMessage(msg);
        msg.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
        msg.payload.widget.data.value = IDX(town->troopTypes[i]) + 1;
        msg.payload.widget.id = i + TOWN_EDIT_FIRST_TROOP_TYPE;
        gEditDialog->BroadcastMessage(msg);
    }
    msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
    msg.payload.widget.id = TOWN_EDIT_CUSTOM_NAME_TOGGLE;
    msg.payload.widget.command =
        town->hasCustomName ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    gEditDialog->BroadcastMessage(msg);
    msg.payload.widget.id = TOWN_EDIT_STANDARD_NAME_TOGGLE;
    msg.payload.widget.command =
        town->hasCustomName ? WIDGET_COMMAND_CLEAR_FLAGS : WIDGET_COMMAND_SET_FLAGS;
    gEditDialog->BroadcastMessage(msg);
    msg.payload.widget.id = TOWN_EDIT_NAME;
    msg.payload.widget.command =
        town->hasCustomName ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    gEditDialog->BroadcastMessage(msg);
    sprintf(text, "%s", town->name);
    msg.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    msg.payload.widget.id = TOWN_EDIT_NAME;
    msg.payload.widget.data.text = text;
    gEditDialog->BroadcastMessage(msg);
    msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
    msg.payload.widget.id = TOWN_EDIT_CAPTAIN;
    msg.payload.widget.command =
        town->hasCaptain ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    gEditDialog->BroadcastMessage(msg);
    if (town->isCastle) {
        msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
        msg.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        msg.payload.widget.id = TOWN_EDIT_ALLOW_CASTLE;
        gEditDialog->BroadcastMessage(msg);
        msg.payload.widget.id = TOWN_EDIT_CASTLE_TEXT;
        gEditDialog->BroadcastMessage(msg);
        msg.payload.widget.id = TOWN_EDIT_CASTLE_LABEL;
        gEditDialog->BroadcastMessage(msg);
    } else {
        msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
        msg.payload.widget.id = TOWN_EDIT_ALLOW_CASTLE;
        msg.payload.widget.command = (1 - town->disallowCastle) ? WIDGET_COMMAND_SET_FLAGS
                                                                : WIDGET_COMMAND_CLEAR_FLAGS;
        gEditDialog->BroadcastMessage(msg);
    }
    msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
    msg.payload.widget.id = TOWN_EDIT_CUSTOM_BUILDINGS_TOGGLE;
    msg.payload.widget.command =
        town->hasCustomBuildings ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    gEditDialog->BroadcastMessage(msg);
    msg.payload.widget.id = TOWN_EDIT_STANDARD_BUILDINGS_TOGGLE;
    msg.payload.widget.command =
        town->hasCustomBuildings ? WIDGET_COMMAND_CLEAR_FLAGS : WIDGET_COMMAND_SET_FLAGS;
    gEditDialog->BroadcastMessage(msg);
    for (i = TOWN_EDIT_FIRST_BUILDING_ROW; i <= TOWN_EDIT_LAST_BUILDING_ROW; i++) {
        msg.payload.widget.id = i;
        msg.payload.widget.command =
            town->hasCustomBuildings ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
        gEditDialog->BroadcastMessage(msg);
    }
    msg.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
    msg.payload.widget.data.value = town->mageGuildLevel;
    msg.payload.widget.id = TOWN_EDIT_MAGE_GUILD;
    gEditDialog->BroadcastMessage(msg);
    msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
    for (i = 0; i < TOWN_EDIT_DWELLING_COUNT; i++) {
        msg.payload.widget.id = i * TOWN_EDIT_DWELLING_STRIDE + TOWN_EDIT_FIRST_DWELLING;
        msg.payload.widget.command =
            town->hasCustomBuildings
                    && (town->buildings & (1 << (i + IDX(BUILDING_SLOT_DWELLING_FIRST))))
                ? WIDGET_COMMAND_SET_FLAGS
                : WIDGET_COMMAND_CLEAR_FLAGS;
        gEditDialog->BroadcastMessage(msg);
        msg.payload.widget.id = i * TOWN_EDIT_DWELLING_STRIDE + TOWN_EDIT_FIRST_DWELLING + 1;
        msg.payload.widget.command =
            town->hasCustomBuildings
                    && (town->buildings & (1 << (i + IDX(BUILDING_SLOT_DWELLING_SIXTH))))
                ? WIDGET_COMMAND_SET_FLAGS
                : WIDGET_COMMAND_CLEAR_FLAGS;
        gEditDialog->BroadcastMessage(msg);
    }
    for (i = 0; i < TOWN_EDIT_BUILDING_COUNT; i++) {
        msg.payload.widget.id = i + TOWN_EDIT_FIRST_BUILDING;
        msg.payload.widget.command =
            town->hasCustomBuildings && (town->buildings & BIT(gTownEditBuildings[i]))
                ? WIDGET_COMMAND_SET_FLAGS
                : WIDGET_COMMAND_CLEAR_FLAGS;
        gEditDialog->BroadcastMessage(msg);
    }
    if (gTownEdit.faction < FACTION_COUNT) {
        for (i = IDX(BUILDING_SLOT_UPGRADE_FIRST); i <= IDX(BUILDING_SLOT_UPGRADE_LAST); i++) {
            if (!(gTownEligibleBuildMask[IDX(gTownEdit.faction)] & (1 << i))) {
                msg.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
                index = i - IDX(BUILDING_SLOT_DWELLING_SIXTH);
                msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
                msg.payload.widget.id =
                    index * TOWN_EDIT_DWELLING_STRIDE + TOWN_EDIT_FIRST_DWELLING + 1;
                gEditDialog->BroadcastMessage(msg);
                msg.payload.widget.id = index * TOWN_EDIT_DWELLING_STRIDE + TOWN_EDIT_FIRST_DWELLING
                                        + 1 - TOWN_EDIT_LABEL_STEP;
                gEditDialog->BroadcastMessage(msg);
                msg.payload.widget.id = index * TOWN_EDIT_DWELLING_STRIDE + TOWN_EDIT_FIRST_DWELLING
                                        + 1 - 2 * TOWN_EDIT_LABEL_STEP;
                gEditDialog->BroadcastMessage(msg);
            }
        }
        // A necromancer town has no tavern.
        if (gTownEdit.faction == FACTION_NECROMANCER) {
            msg.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            msg.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
            msg.payload.widget.id = TOWN_EDIT_TAVERN;
            gEditDialog->BroadcastMessage(msg);
            msg.payload.widget.id = TOWN_EDIT_TAVERN - TOWN_EDIT_LABEL_STEP;
            gEditDialog->BroadcastMessage(msg);
            msg.payload.widget.id = TOWN_EDIT_TAVERN - 2 * TOWN_EDIT_LABEL_STEP;
            gEditDialog->BroadcastMessage(msg);
        }
    }
}

VA(0x0042a930, 0x5c3)
MessageDispatchResult EditTownHandler(tag_message& message) {
    i32 building;
    i32 present;
    b32 update;
    i32 number;
    // Never read: slots of the retail frame, the cell editor's handler
    // constants.
    const i16 H2_UNUSED(firstTextId) = 1;
    const i16 H2_UNUSED(toggleBase) = CELL_WINDOW_FIRST_FLAG;

    update = false;
    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_NOTIFY_DESELECT:
                    switch (message.payload.widget.id) {
                        case EVENTS_DIALOG_CANCEL:
                        case EVENTS_DIALOG_OK:
                            gpWindowManager->m_dialogResult = message.payload.widget.id;
                            FINISH_EDIT_DIALOG(message);
                            return MESSAGE_DISPATCH_FORWARD;
                    }
                    break;
                case WIDGET_NOTIFY_SELECT:
                    switch (message.payload.widget.id) {
                        case TOWN_EDIT_FIRST_TROOP_COUNT:
                        case TOWN_EDIT_FIRST_TROOP_COUNT + 1:
                        case TOWN_EDIT_FIRST_TROOP_COUNT + 2:
                        case TOWN_EDIT_FIRST_TROOP_COUNT + 3:
                        case TOWN_EDIT_FIRST_TROOP_COUNT + 4:
                            message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
                            gEditDialog->BroadcastMessage(message);
                            number = atoi(message.payload.widget.data.text);
                            if (number < 0)
                                number = 0;
                            if (number > TOWN_EDIT_MAX_TROOP_COUNT)
                                number = TOWN_EDIT_MAX_TROOP_COUNT;
                            gTownEdit.troopCounts[message.payload.widget.id
                                                  - TOWN_EDIT_FIRST_TROOP_COUNT] = number;
                            update = true;
                            break;
                        case TOWN_EDIT_FIRST_TROOP_TYPE:
                        case TOWN_EDIT_FIRST_TROOP_TYPE + 1:
                        case TOWN_EDIT_FIRST_TROOP_TYPE + 2:
                        case TOWN_EDIT_FIRST_TROOP_TYPE + 3:
                        case TOWN_EDIT_FIRST_TROOP_TYPE + 4:
                            message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                            gEditDialog->BroadcastMessage(message);
                            gTownEdit.troopTypes[message.payload.widget.id
                                                 - TOWN_EDIT_FIRST_TROOP_TYPE] =
                                message.payload.widget.data.value - 1;
                            update = true;
                            break;
                        case TOWN_EDIT_MAGE_GUILD:
                            message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                            gEditDialog->BroadcastMessage(message);
                            gTownEdit.mageGuildLevel = message.payload.widget.data.value;
                            if (gTownEdit.mageGuildLevel)
                                gTownEdit.buildings |= 1 << IDX(BUILDING_SLOT_MAGE_GUILD);
                            else
                                gTownEdit.buildings &= ~(1 << IDX(BUILDING_SLOT_MAGE_GUILD));
                            update = true;
                            break;
                        case TOWN_EDIT_FIRST_DWELLING:
                        case TOWN_EDIT_FIRST_DWELLING + 2:
                        case TOWN_EDIT_FIRST_DWELLING + 4:
                        case TOWN_EDIT_FIRST_DWELLING + 6:
                        case TOWN_EDIT_FIRST_DWELLING + 8:
                        case TOWN_EDIT_FIRST_DWELLING + 10:
                            building = 1 << (((message.payload.widget.id - TOWN_EDIT_FIRST_DWELLING) >> 1)
                                             + IDX(BUILDING_SLOT_DWELLING_FIRST));
                            goto toggleBuilding;
                        case TOWN_EDIT_FIRST_DWELLING + 1:
                        case TOWN_EDIT_FIRST_DWELLING + 3:
                        case TOWN_EDIT_FIRST_DWELLING + 5:
                        case TOWN_EDIT_FIRST_DWELLING + 7:
                        case TOWN_EDIT_FIRST_DWELLING + 9:
                        case TOWN_EDIT_FIRST_DWELLING + 11:
                            building = 1 << (((message.payload.widget.id - TOWN_EDIT_FIRST_DWELLING) >> 1)
                                             + IDX(BUILDING_SLOT_DWELLING_SIXTH));
                            goto toggleBuilding;
                        case TOWN_EDIT_FIRST_BUILDING:
                        case TOWN_EDIT_FIRST_BUILDING + 1:
                        case TOWN_EDIT_FIRST_BUILDING + 2:
                        case TOWN_EDIT_FIRST_BUILDING + 3:
                        case TOWN_EDIT_FIRST_BUILDING + 4:
                        case TOWN_EDIT_FIRST_BUILDING + 5:
                        case TOWN_EDIT_FIRST_BUILDING + 6:
                        case TOWN_EDIT_FIRST_BUILDING + 7:
                        case TOWN_EDIT_FIRST_BUILDING + 8:
                        case TOWN_EDIT_FIRST_BUILDING + 9:
                        case TOWN_EDIT_FIRST_BUILDING + 10:
                            number = message.payload.widget.id - TOWN_EDIT_FIRST_BUILDING;
                            building = BIT(gTownEditBuildings[number]);
                        toggleBuilding:
                            present = gTownEdit.buildings & building;
                            if (present)
                                gTownEdit.buildings -= building;
                            else
                                gTownEdit.buildings += building;
                            update = true;
                            break;
                        case TOWN_EDIT_CAPTAIN:
                            gTownEdit.hasCaptain = 1 - gTownEdit.hasCaptain;
                            update = true;
                            break;
                        case TOWN_EDIT_ALLOW_CASTLE:
                            gTownEdit.disallowCastle = 1 - gTownEdit.disallowCastle;
                            update = true;
                            break;
                        case TOWN_EDIT_STANDARD_ARMY:
                        case TOWN_EDIT_STANDARD_ARMY_TOGGLE:
                            gTownEdit.hasCustomArmy = false;
                            update = true;
                            break;
                        case TOWN_EDIT_CUSTOM_ARMY:
                        case TOWN_EDIT_CUSTOM_ARMY_TOGGLE:
                            gTownEdit.hasCustomArmy = true;
                            update = true;
                            break;
                        case TOWN_EDIT_STANDARD_BUILDINGS:
                        case TOWN_EDIT_STANDARD_BUILDINGS_TOGGLE:
                            gTownEdit.hasCustomBuildings = false;
                            update = true;
                            break;
                        case TOWN_EDIT_CUSTOM_BUILDINGS:
                        case TOWN_EDIT_CUSTOM_BUILDINGS_TOGGLE:
                            gTownEdit.hasCustomBuildings = true;
                            update = true;
                            break;
                        case TOWN_EDIT_STANDARD_NAME:
                        case TOWN_EDIT_STANDARD_NAME_TOGGLE:
                            gTownEdit.hasCustomName = false;
                            update = true;
                            break;
                        case TOWN_EDIT_CUSTOM_NAME:
                        case TOWN_EDIT_CUSTOM_NAME_TOGGLE:
                            gTownEdit.hasCustomName = true;
                            update = true;
                            break;
                        case TOWN_EDIT_NAME:
                            message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
                            gEditDialog->BroadcastMessage(message);
                            strcpy(gTownEdit.name, message.payload.widget.data.text);
                            update = true;
                            break;
                    }
                    break;
            }
            break;
        case MESSAGE_KEY_DOWN:
            switch (message.payload.keyboard.keyCode) {
                case INPUT_SCAN_ESCAPE:
                    FINISH_EDIT_DIALOG(message);
                    return MESSAGE_DISPATCH_FORWARD;
            }
            break;
    }
    if (update) {
        static_cast<eventsManager*>(gEditManager->m_toolManager)->FillInTownEdit(&gTownEdit);
        gEditDialog->DrawWindow();
    }
    return MESSAGE_DISPATCH_CONSUME;
}
