// The hero editor (heroedit.bin): eventsManager::EditHero, the dialog's
// fill-in and its handler. The unit name is descriptive: the dialog
// resource names it.

#include <va.h>
#include <EDITOR/heroedit.h>
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

// Zero-initialized: .bss in definition order.
DATA(0x004a4c50) HeroExtra gHeroEdit = {0};
DATA(0x004a4c9c) b32 gEditJailedHero = false;
// Unread zero-initialized storage after gEditJailedHero.
DATA(0x004a4ca0) i32 gUnusedData4a4ca0[2] = {0};

VA(0x00413f50, 0x5c1)
void eventsManager::EditHero(i32 x, i32 y, b32 jailed) {
    // Never read: a copy of the cell.
    mapCell original;
    i32 i;
    i32 j;
    i32 k;
    tag_message msg;

    gEditJailedHero = jailed;
    gEditCell = gMap.GetCell(x, y);
    original = *gEditCell;
    memcpy(&gHeroEdit, gEditManager->m_extras[gEditCell->m_objectMetadata], sizeof(HeroExtra));
    gEditDialog = new heroWindow(0, 0, "heroedit.bin");
    SetWinText(gEditDialog, HERO_EDIT_TEXT_ROW);
    if (gEditJailedHero) {
        SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_SET_TEXT, HERO_EDIT_TYPE_LABEL);
        msg.payload.widget.data.text = localization::Tr("editor.events.hero.class_label");
        gEditDialog->BroadcastMessage(msg);
    }
    msg.type = MESSAGE_WIDGET;
    msg.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
    for (i = 0; i < HERO_SECONDARY_SKILL_CAPACITY; i++) {
        sprintf(gText, localization::Tr("editor.events.empty_entry"));
        msg.payload.widget.data.text = gText;
        msg.payload.widget.id = i + HERO_EDIT_FIRST_SKILL;
        gEditDialog->BroadcastMessage(msg);
        for (j = 0; j < HERO_EDIT_SKILL_LEVELS; j++) {
            for (k = 0; k < IDX(HERO_SKILL_COUNT); k++) {
                sprintf(gText, "%s %s", gSecondarySkillLevels[j], gSecondarySkills[k]);
                msg.payload.widget.data.text = gText;
                msg.payload.widget.id = i + HERO_EDIT_FIRST_SKILL;
                gEditDialog->BroadcastMessage(msg);
            }
        }
    }
    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        sprintf(gText, localization::Tr("editor.events.empty_entry"));
        msg.payload.widget.data.text = gText;
        msg.payload.widget.id = i + HERO_EDIT_FIRST_TROOP_TYPE;
        gEditDialog->BroadcastMessage(msg);
        for (k = 0; k < IDX(CREATURE_COUNT); k++) {
            sprintf(gText, "%s", gArmyNames[k]);
            gText[0] = CyrillicToUpper(gText[0]);
            msg.payload.widget.data.text = gText;
            msg.payload.widget.id = i + HERO_EDIT_FIRST_TROOP_TYPE;
            gEditDialog->BroadcastMessage(msg);
        }
    }
    for (i = 0; i < EVENT_RECORD_HERO_ARTIFACT_COUNT; i++) {
        sprintf(gText, localization::Tr("editor.events.empty_entry"));
        msg.payload.widget.data.text = gText;
        msg.payload.widget.id = i + HERO_EDIT_FIRST_ARTIFACT;
        gEditDialog->BroadcastMessage(msg);
        for (k = 0; k < IDX(ARTIFACT_COUNT); k++) {
            if (k < IDX(ARTIFACT_EDITOR_ANY_ULTIMATE) || k > IDX(ARTIFACT_SPELL_SCROLL)) {
                sprintf(gText, "%s", gArtifactNames[k]);
                msg.type = MESSAGE_WIDGET;
                msg.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
                msg.payload.widget.data.text = gText;
                msg.payload.widget.id = i + HERO_EDIT_FIRST_ARTIFACT;
                gEditDialog->BroadcastMessage(msg);
            }
        }
    }
    if (gEditJailedHero) {
        for (k = 0; k < HERO_EDIT_CLASS_COUNT; k++) {
            sprintf(gText, gAlignmentNames[k]);
            msg.type = MESSAGE_WIDGET;
            msg.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
            msg.payload.widget.id = HERO_EDIT_PATROL_RADIUS;
            msg.payload.widget.data.text = gText;
            gEditDialog->BroadcastMessage(msg);
        }
    } else {
        sprintf(gText, localization::Tr("editor.events.hero.stand_still"));
        msg.payload.widget.data.text = gText;
        msg.payload.widget.id = HERO_EDIT_PATROL_RADIUS;
        gEditDialog->BroadcastMessage(msg);
        for (k = 1; k <= HERO_EDIT_MAX_PATROL_RADIUS; k++) {
            if (k == 1)
                sprintf(gText, localization::Tr("editor.events.hero.patrol_radius_one"), k);
            else
                sprintf(gText, localization::Tr("editor.events.hero.patrol_radius"), k);
            msg.type = MESSAGE_WIDGET;
            msg.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
            msg.payload.widget.data.text = gText;
            msg.payload.widget.id = HERO_EDIT_PATROL_RADIUS;
            gEditDialog->BroadcastMessage(msg);
        }
    }
    FillInHeroEdit(&gHeroEdit);
    gpWindowManager->DoDialog(gEditDialog, HeroEditHandler, 0);
    delete gEditDialog;
    if (gpWindowManager->m_dialogResult != EVENTS_DIALOG_CANCEL) {
        gEditManager->m_mapChanged = 1;
        memcpy(gEditManager->m_extras[gEditCell->m_objectMetadata], &gHeroEdit, sizeof(HeroExtra));
    }
    gEditManager->DrawMap();
    gEditManager->UpdateMapView();
}

VA(0x00414511, 0x4c2)
void eventsManager::FillInHeroEdit(HeroExtra* hero) {
    char text[HERO_EDIT_TEXT_SIZE];
    tag_message message;
    i32 i;

    message.type = MESSAGE_WIDGET;
    message.payload.widget.data.value = WIDGET_FLAG_DRAW;
    message.payload.widget.id = HERO_EDIT_CUSTOM_ARMY_TOGGLE;
    message.payload.widget.command =
        hero->hasCustomArmy ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.id = HERO_EDIT_STANDARD_ARMY_TOGGLE;
    message.payload.widget.command =
        hero->hasCustomArmy ? WIDGET_COMMAND_CLEAR_FLAGS : WIDGET_COMMAND_SET_FLAGS;
    gEditDialog->BroadcastMessage(message);
    for (i = HERO_EDIT_FIRST_ARMY_ROW; i <= HERO_EDIT_LAST_ARMY_ROW; i++) {
        message.payload.widget.id = i;
        message.payload.widget.command =
            hero->hasCustomArmy ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
        gEditDialog->BroadcastMessage(message);
    }
    if (gEditJailedHero) {
        message.payload.widget.id = HERO_EDIT_PATROL_RADIUS;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        gEditDialog->BroadcastMessage(message);
    } else {
        message.payload.widget.id = HERO_EDIT_PATROL_RADIUS;
        message.payload.widget.command =
            hero->hasPatrol ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
        gEditDialog->BroadcastMessage(message);
    }
    if (gEditJailedHero) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.id = HERO_EDIT_PATROL;
        gEditDialog->BroadcastMessage(message);
        message.payload.widget.id = HERO_EDIT_CLASS_LABEL;
        gEditDialog->BroadcastMessage(message);
    } else {
        message.payload.widget.id = HERO_EDIT_PATROL;
        gEditDialog->BroadcastMessage(message);
    }
    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        sprintf(text, "%d", hero->troopCounts[i]);
        message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        message.payload.widget.data.text = text;
        message.payload.widget.id = i + HERO_EDIT_FIRST_TROOP_COUNT;
        gEditDialog->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
        message.payload.widget.data.value = hero->troopTypes[i] + 1;
        message.payload.widget.id = i + HERO_EDIT_FIRST_TROOP_TYPE;
        gEditDialog->BroadcastMessage(message);
    }
    for (i = 0; i < EVENT_RECORD_HERO_ARTIFACT_COUNT; i++) {
        message.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
        message.payload.widget.data.value = hero->artifacts[i] + 1;
        if (hero->artifacts[i] >= IDX(ARTIFACT_EDITOR_ANY_ULTIMATE))
            message.payload.widget.data.value -= HERO_EDIT_HIDDEN_ARTIFACTS;
        message.payload.widget.id = i + HERO_EDIT_FIRST_ARTIFACT;
        gEditDialog->BroadcastMessage(message);
    }
    message.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
    message.payload.widget.data.value = hero->patrolRadius;
    message.payload.widget.id = HERO_EDIT_PATROL_RADIUS;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", hero->experience);
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = HERO_EDIT_EXPERIENCE;
    message.payload.widget.data.text = text;
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.data.value = WIDGET_FLAG_DRAW;
    message.payload.widget.id = HERO_EDIT_CUSTOM_SKILLS_TOGGLE;
    message.payload.widget.command =
        hero->hasCustomSkills ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.id = HERO_EDIT_STANDARD_SKILLS_TOGGLE;
    message.payload.widget.command =
        hero->hasCustomSkills ? WIDGET_COMMAND_CLEAR_FLAGS : WIDGET_COMMAND_SET_FLAGS;
    gEditDialog->BroadcastMessage(message);
    for (i = HERO_EDIT_FIRST_SKILL_ROW; i <= HERO_EDIT_LAST_SKILL_ROW; i++) {
        message.payload.widget.id = i;
        message.payload.widget.command =
            hero->hasCustomSkills ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
        gEditDialog->BroadcastMessage(message);
    }
    for (i = 0; i < HERO_SECONDARY_SKILL_CAPACITY; i++) {
        message.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
        message.payload.widget.id = i + HERO_EDIT_FIRST_SKILL;
        if (hero->skillTypes[i] == -1)
            message.payload.widget.data.value = 0;
        else
            message.payload.widget.data.value =
                hero->skillTypes[i] + (hero->skillLevels[i] - 1) * IDX(HERO_SKILL_COUNT) + 1;
        gEditDialog->BroadcastMessage(message);
    }
    message.payload.widget.data.value = WIDGET_FLAG_DRAW;
    message.payload.widget.id = HERO_EDIT_CUSTOM_NAME_TOGGLE;
    message.payload.widget.command =
        hero->hasCustomName ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.id = HERO_EDIT_STANDARD_NAME_TOGGLE;
    message.payload.widget.command =
        hero->hasCustomName ? WIDGET_COMMAND_CLEAR_FLAGS : WIDGET_COMMAND_SET_FLAGS;
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.id = HERO_EDIT_NAME;
    message.payload.widget.command =
        hero->hasCustomName ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%s", hero->name);
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = HERO_EDIT_NAME;
    message.payload.widget.data.text = text;
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    message.payload.widget.id = HERO_EDIT_PORTRAIT;
    message.payload.widget.data.value = hero->hasCustomPortrait ? hero->portrait + 1 : 0;
    gEditDialog->BroadcastMessage(message);
}

VA(0x004149d3, 0x6ba)
MessageDispatchResult HeroEditHandler(tag_message& message) {
    i32 number;
    b32 update;

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
                        case HERO_EDIT_PREVIOUS_PORTRAIT:
                            if (!gHeroEdit.hasCustomPortrait)
                                break;
                            if (!gHeroEdit.portrait) {
                                gHeroEdit.portrait = -1;
                                gHeroEdit.hasCustomPortrait = false;
                            } else
                                gHeroEdit.portrait--;
                            update = true;
                            break;
                        case HERO_EDIT_NEXT_PORTRAIT:
                            if (!gHeroEdit.hasCustomPortrait) {
                                gHeroEdit.hasCustomPortrait = true;
                                gHeroEdit.portrait = 0;
                            } else if (gHeroEdit.portrait < HERO_EDIT_LAST_PORTRAIT)
                                gHeroEdit.portrait++;
                            update = true;
                            break;
                    }
                    break;
                case WIDGET_NOTIFY_SELECT:
                    switch (message.payload.widget.id) {
                        case HERO_EDIT_FIRST_TROOP_COUNT:
                        case HERO_EDIT_FIRST_TROOP_COUNT + 1:
                        case HERO_EDIT_FIRST_TROOP_COUNT + 2:
                        case HERO_EDIT_FIRST_TROOP_COUNT + 3:
                        case HERO_EDIT_FIRST_TROOP_COUNT + 4:
                            message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
                            gEditDialog->BroadcastMessage(message);
                            number = atoi(message.payload.widget.data.text);
                            if (number < 0)
                                number = 0;
                            if (number > HERO_EDIT_MAX_TROOP_COUNT)
                                number = HERO_EDIT_MAX_TROOP_COUNT;
                            gHeroEdit.troopCounts[message.payload.widget.id
                                                  - HERO_EDIT_FIRST_TROOP_COUNT] = number;
                            update = true;
                            break;
                        case HERO_EDIT_FIRST_TROOP_TYPE:
                        case HERO_EDIT_FIRST_TROOP_TYPE + 1:
                        case HERO_EDIT_FIRST_TROOP_TYPE + 2:
                        case HERO_EDIT_FIRST_TROOP_TYPE + 3:
                        case HERO_EDIT_FIRST_TROOP_TYPE + 4:
                            message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                            gEditDialog->BroadcastMessage(message);
                            gHeroEdit.troopTypes[message.payload.widget.id
                                                 - HERO_EDIT_FIRST_TROOP_TYPE] =
                                message.payload.widget.data.value - 1;
                            update = true;
                            break;
                        // The list holds three artifacts; the fourth id is never sent.
                        case HERO_EDIT_FIRST_ARTIFACT:
                        case HERO_EDIT_FIRST_ARTIFACT + 1:
                        case HERO_EDIT_FIRST_ARTIFACT + 2:
                        case HERO_EDIT_FIRST_ARTIFACT + 3:
                            message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                            gEditDialog->BroadcastMessage(message);
                            if (message.payload.widget.data.value - 1
                                >= IDX(ARTIFACT_EDITOR_ANY_ULTIMATE))
                                message.payload.widget.data.value += HERO_EDIT_HIDDEN_ARTIFACTS;
                            gHeroEdit.artifacts[message.payload.widget.id - HERO_EDIT_FIRST_ARTIFACT] =
                                message.payload.widget.data.value - 1;
                            update = true;
                            break;
                        case HERO_EDIT_EXPERIENCE:
                            message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
                            gEditDialog->BroadcastMessage(message);
                            number = atoi(message.payload.widget.data.text);
                            if (number < 0)
                                number = 0;
                            if (number > HERO_EDIT_MAX_EXPERIENCE)
                                number = HERO_EDIT_MAX_EXPERIENCE;
                            gHeroEdit.experience = number;
                            update = true;
                            break;
                        case HERO_EDIT_PATROL:
                            gHeroEdit.hasPatrol = 1 - gHeroEdit.hasPatrol;
                            update = true;
                            break;
                        case HERO_EDIT_PATROL_RADIUS:
                            message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                            gEditDialog->BroadcastMessage(message);
                            gHeroEdit.patrolRadius = message.payload.widget.data.value;
                            update = true;
                            break;
                        case HERO_EDIT_FIRST_SKILL:
                        case HERO_EDIT_FIRST_SKILL + 1:
                        case HERO_EDIT_FIRST_SKILL + 2:
                        case HERO_EDIT_FIRST_SKILL + 3:
                        case HERO_EDIT_FIRST_SKILL + 4:
                        case HERO_EDIT_FIRST_SKILL + 5:
                        case HERO_EDIT_FIRST_SKILL + 6:
                        case HERO_EDIT_FIRST_SKILL + 7:
                            message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                            gEditDialog->BroadcastMessage(message);
                            if (message.payload.widget.data.value == 0) {
                                gHeroEdit.skillTypes[message.payload.widget.id
                                                     - HERO_EDIT_FIRST_SKILL] = -1;
                                gHeroEdit.skillLevels[message.payload.widget.id
                                                      - HERO_EDIT_FIRST_SKILL] = 0;
                            } else {
                                gHeroEdit.skillTypes[message.payload.widget.id
                                                     - HERO_EDIT_FIRST_SKILL] =
                                    (message.payload.widget.data.value - 1) % IDX(HERO_SKILL_COUNT);
                                gHeroEdit.skillLevels[message.payload.widget.id
                                                      - HERO_EDIT_FIRST_SKILL] =
                                    (message.payload.widget.data.value - 1) / IDX(HERO_SKILL_COUNT)
                                    + 1;
                            }
                            update = true;
                            break;
                        case HERO_EDIT_NAME:
                            message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
                            gEditDialog->BroadcastMessage(message);
                            strcpy(gHeroEdit.name, message.payload.widget.data.text);
                            update = true;
                            break;
                        case HERO_EDIT_STANDARD_ARMY:
                        case HERO_EDIT_STANDARD_ARMY_TOGGLE:
                            gHeroEdit.hasCustomArmy = false;
                            update = true;
                            break;
                        case HERO_EDIT_CUSTOM_ARMY:
                        case HERO_EDIT_CUSTOM_ARMY_TOGGLE:
                            gHeroEdit.hasCustomArmy = true;
                            update = true;
                            break;
                        case HERO_EDIT_STANDARD_SKILLS:
                        case HERO_EDIT_STANDARD_SKILLS_TOGGLE:
                            gHeroEdit.hasCustomSkills = false;
                            update = true;
                            break;
                        case HERO_EDIT_CUSTOM_SKILLS:
                        case HERO_EDIT_CUSTOM_SKILLS_TOGGLE:
                            gHeroEdit.hasCustomSkills = true;
                            update = true;
                            break;
                        case HERO_EDIT_STANDARD_NAME:
                        case HERO_EDIT_STANDARD_NAME_TOGGLE:
                            gHeroEdit.hasCustomName = false;
                            update = true;
                            break;
                        case HERO_EDIT_CUSTOM_NAME:
                        case HERO_EDIT_CUSTOM_NAME_TOGGLE:
                            gHeroEdit.hasCustomName = true;
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
        static_cast<eventsManager*>(gEditManager->m_toolManager)->FillInHeroEdit(&gHeroEdit);
        gEditDialog->DrawWindow();
    }
    return MESSAGE_DISPATCH_CONSUME;
}
