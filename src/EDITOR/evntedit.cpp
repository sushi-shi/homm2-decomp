// The event editor (evntedit.bin): eventsManager::EditEvent, the dialog's
// fill-in and its handler. Retail evidence: the editor's evntedit.bin
// resource and the Price of Loyalty editor's evntedit.cpp.

#include <match.h>
#include <EDITOR/evntedit.h>
#include <EDITOR/eventsManager.h>
#include <EDITOR/editManager.h>
#include <EDITOR/EDITOR.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/message.h>
#include <BASE/widget.h>
#include <SOURCE/gameTypes.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Zero-initialized: .bss in definition order.
DATA(0x004a4c18) EventExtra gEventEdit = {0};
DATA(0x004a4c4c) char* gEventMessage = NULL;

VA(0x004134b0, 0x392)
i32 eventsManager::EditEvent(i32 extra) {
    i32 control;
    // Never read: a slot of the retail frame.
    i32 H2_UNUSED(unused);
    i32 byteCount;
    tag_message message;
    i32 i;
    char* newExtra;

    ResetPlayerAvailability();
    memcpy(&gEventEdit, gEditManager->m_extras[extra], sizeof(EventExtra));
    gEventMessage = new char[EVENT_TEXT_CAPACITY];
    strcpy(gEventMessage, static_cast<EventExtra*>(gEditManager->m_extras[extra])->message);
    gEditDialog = new heroWindow(0, 0, "evntedit.bin");
    SetWinText(gEditDialog, EDITOR_WIN_TEXT_EVENT);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
    sprintf(gText, localization::Tr("editor.events.event.no_artifact"));
    message.payload.widget.data.text = gText;
    message.payload.widget.id = EVENT_EDIT_ARTIFACT;
    gEditDialog->BroadcastMessage(message);
    for (i = 0; i < IDX(ARTIFACT_COUNT); i++) {
        if (i < IDX(ARTIFACT_EDITOR_ANY_ULTIMATE) || i > IDX(ARTIFACT_SPELL_SCROLL)) {
            sprintf(gText, "%s", gArtifactNames[i]);
            message.payload.widget.data.text = gText;
            gEditDialog->BroadcastMessage(message);
        }
    }
    message.payload.widget.id = EVENT_EDIT_FREQUENCY;
    for (i = 0; i < EVENT_FREQUENCY_COUNT; i++) {
        sprintf(gText, "%s", gEventFrequencyNames[i]);
        message.payload.widget.data.text = gText;
        gEditDialog->BroadcastMessage(message);
    }
    message.type = MESSAGE_WIDGET;
    message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
    for (control = EVENT_EDIT_FIRST_MAP_ROW; control < EVENT_EDIT_MAP_ROW_END; control++) {
        message.payload.widget.id = control;
        message.payload.widget.command =
            gEventEdit.isMapEvent ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
        gEditDialog->BroadcastMessage(message);
    }
    for (control = EVENT_EDIT_FIRST_TIME_ROW; control < EVENT_EDIT_TIME_ROW_END; control++) {
        message.payload.widget.id = control;
        message.payload.widget.command =
            gEventEdit.isMapEvent ? WIDGET_COMMAND_CLEAR_FLAGS : WIDGET_COMMAND_SET_FLAGS;
        gEditDialog->BroadcastMessage(message);
    }
    FillInEventEdit(&gEventEdit);
    gpWindowManager->DoDialog(gEditDialog, EditEventHandler, false);
    delete gEditDialog;
    if (gpWindowManager->m_dialogResult != EVENTS_DIALOG_CANCEL) {
        byteCount = strlen(gEventMessage) + sizeof(EventExtra);
        newExtra = new char[byteCount];
        memcpy(newExtra, &gEventEdit, sizeof(EventExtra));
        strcpy(reinterpret_cast<EventExtra*>(newExtra)->message, gEventMessage);
        delete[] static_cast<char*>(gEditManager->m_extras[extra]);
        gEditManager->m_extras[extra] = newExtra;
        gEditManager->m_extraSizes[extra] = byteCount;
        delete[] gEventMessage;
        gEventMessage = NULL;
        gEditManager->m_mapChanged = true;
    }
    gEditManager->UpdateMapView();
    return gpWindowManager->m_dialogResult;
}

VA(0x00413842, 0x288)
void eventsManager::FillInEventEdit(EventExtra* event) {
    char text[EVENTS_FIELD_TEXT_SIZE];
    tag_message message;
    i32 i;

    message.type = MESSAGE_WIDGET;
    message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
    for (i = 0; i < GAME_PLAYER_COUNT; i++) {
        message.payload.widget.id = i + EVENT_EDIT_FIRST_PLAYER;
        message.payload.widget.command = gEditMapHeader.playerEnabled[i]
                                             ? WIDGET_COMMAND_SET_FLAGS
                                             : WIDGET_COMMAND_CLEAR_FLAGS;
        gEditDialog->BroadcastMessage(message);
        message.payload.widget.id = i + EVENT_EDIT_FIRST_PLAYER_TOGGLE;
        message.payload.widget.command = gEditMapHeader.playerEnabled[i] && event->players[i]
                                             ? WIDGET_COMMAND_SET_FLAGS
                                             : WIDGET_COMMAND_CLEAR_FLAGS;
        gEditDialog->BroadcastMessage(message);
    }
    message.payload.widget.id = EVENT_EDIT_CANCEL_AFTER_VISIT;
    message.payload.widget.command = gEventEdit.isMapEvent && event->cancelAfterVisit
                                         ? WIDGET_COMMAND_SET_FLAGS
                                         : WIDGET_COMMAND_CLEAR_FLAGS;
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.id = EVENT_EDIT_APPLY_TO_COMPUTER;
    message.payload.widget.command =
        event->appliesToComputer ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.id = EVENT_EDIT_APPLY_TO_HUMAN;
    message.payload.widget.command =
        event->appliesToHuman ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.data.text = text;
    for (i = 0; i < IDX(RES_COUNT); i++) {
        sprintf(text, "%d", gEventEdit.resources[i]);
        message.payload.widget.id = i + EVENT_EDIT_FIRST_RESOURCE;
        gEditDialog->BroadcastMessage(message);
    }
    sprintf(text, "%d", gEventEdit.firstDay);
    message.payload.widget.id = EVENT_EDIT_FIRST_DAY;
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.data.text = gEventMessage;
    message.payload.widget.id = EVENT_TEXT_FIELD;
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
    message.payload.widget.data.value = gEventEdit.artifact + 1;
    if (gEventEdit.artifact >= IDX(ARTIFACT_EDITOR_ANY_ULTIMATE))
        message.payload.widget.data.value -= EVENTS_HIDDEN_ARTIFACT_COUNT;
    message.payload.widget.id = EVENT_EDIT_ARTIFACT;
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.id = EVENT_EDIT_FREQUENCY;
    message.payload.widget.data.value =
        gEventEdit.repeatInterval <= EVENT_EDIT_DAILY_LAST
            ? gEventEdit.repeatInterval
            : gEventEdit.repeatInterval / CALENDAR_DAYS_PER_WEEK + EVENT_EDIT_WEEKLY_BASE;
    gEditDialog->BroadcastMessage(message);
}

VA(0x00413aca, 0x447)
MessageDispatchResult EditEventHandler(tag_message& message) {
    tag_message query;
    // Never read: slots of the retail frame.
    i32 H2_UNUSED(unused);
    i32 H2_UNUSED(unusedValue);
    b32 update;
    i32 H2_UNUSED(reserved);
    i32 playerIndex;

    update = false;
    query.type = MESSAGE_WIDGET;
    query.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
    query.payload.widget.id = message.payload.widget.id;
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
                    update = true;
                    switch (message.payload.widget.id) {
                        case EVENT_TEXT_FIELD:
                            gEditDialog->BroadcastMessage(query);
                            strcpy(gEventMessage, query.payload.widget.data.text);
                            break;
                        case EVENT_EDIT_CANCEL_AFTER_VISIT:
                            gEventEdit.cancelAfterVisit = 1 - gEventEdit.cancelAfterVisit;
                            break;
                        case EVENT_EDIT_APPLY_TO_COMPUTER:
                            gEventEdit.appliesToComputer = 1 - gEventEdit.appliesToComputer;
                            break;
                        case EVENT_EDIT_APPLY_TO_HUMAN:
                            gEventEdit.appliesToHuman = 1 - gEventEdit.appliesToHuman;
                            break;
                        case EVENT_EDIT_FIRST_PLAYER_TOGGLE:
                        case EVENT_EDIT_FIRST_PLAYER_TOGGLE + 1:
                        case EVENT_EDIT_FIRST_PLAYER_TOGGLE + 2:
                        case EVENT_EDIT_FIRST_PLAYER_TOGGLE + 3:
                        case EVENT_EDIT_FIRST_PLAYER_TOGGLE + 4:
                        case EVENT_EDIT_LAST_PLAYER_TOGGLE:
                            playerIndex =
                                message.payload.widget.id - EVENT_EDIT_FIRST_PLAYER_TOGGLE;
                            gEventEdit.players[playerIndex] = 1 - gEventEdit.players[playerIndex];
                            break;
                        case EVENT_EDIT_ARTIFACT:
                            message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                            gEditDialog->BroadcastMessage(message);
                            if (message.payload.widget.data.value - 1
                                >= IDX(ARTIFACT_EDITOR_ANY_ULTIMATE))
                                message.payload.widget.data.value += EVENTS_HIDDEN_ARTIFACT_COUNT;
                            gEventEdit.artifact = message.payload.widget.data.value - 1;
                            update = true;
                            break;
                        case EVENT_EDIT_FIRST_RESOURCE:
                        case EVENT_EDIT_FIRST_RESOURCE + 1:
                        case EVENT_EDIT_FIRST_RESOURCE + 2:
                        case EVENT_EDIT_FIRST_RESOURCE + 3:
                        case EVENT_EDIT_FIRST_RESOURCE + 4:
                        case EVENT_EDIT_FIRST_RESOURCE + 5:
                        case EVENT_EDIT_LAST_RESOURCE:
                            gEditDialog->BroadcastMessage(query);
                            gEventEdit.resources[message.payload.widget.id
                                                 - EVENT_EDIT_FIRST_RESOURCE] =
                                atoi(query.payload.widget.data.text);
                            break;
                        case EVENT_EDIT_FREQUENCY:
                            message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                            gEditDialog->BroadcastMessage(message);
                            gEventEdit.repeatInterval =
                                message.payload.widget.data.value <= EVENT_EDIT_DAILY_LAST
                                    ? message.payload.widget.data.value
                                    : (message.payload.widget.data.value - EVENT_EDIT_WEEKLY_BASE)
                                          * CALENDAR_DAYS_PER_WEEK;
                            break;
                        case EVENT_EDIT_FIRST_DAY:
                            gEditDialog->BroadcastMessage(query);
                            gEventEdit.firstDay = atoi(query.payload.widget.data.text);
                            break;
                        default:
                            update = false;
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
        static_cast<eventsManager*>(gEditManager->m_toolManager)->FillInEventEdit(&gEventEdit);
        gEditDialog->DrawWindow();
    }
    return MESSAGE_DISPATCH_CONSUME;
}
