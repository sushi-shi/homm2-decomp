// The sphinx editor: the event tool opens it for a sphinx and edits its
// riddle, the accepted answers and the reward. The unit name comes from
// its dialog resource (ridledit.bin); descriptive names: EditSphinx,
// gSphinx, gSphinxText. FillInSphinxEdit takes the name of the Price of
// Loyalty editor's FillInEventEdit family.

#include <va.h>
#include <EDITOR/ridledit.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/editManager.h>
#include <EDITOR/eventsManager.h>
#include <BASE/Misc.h>
#include <BASE/dialog.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/message.h>
#include <BASE/widget.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

H2_ENUM_BEGIN(SphinxDialog)
    SPHINX_WINDOW_TEXT_ID = 10,
    // The reward resources' fields, one per resource.
    SPHINX_RESOURCE_FIRST = 200,
    SPHINX_RESOURCE_LAST  = SPHINX_RESOURCE_FIRST + IDX(RES_COUNT) - 1,
    SPHINX_ARTIFACT_LIST  = 0x12d,
    SPHINX_ANSWER_LIST    = 0x19a,
    SPHINX_ADD_ANSWER     = 0x1a4,
    SPHINX_DELETE_ANSWER  = 0x1ae,
    // The artifact list skips the editor-only artifacts and the spell scroll.
    SPHINX_SKIPPED_ARTIFACTS = IDX(ARTIFACT_SPELL_SCROLL) - IDX(ARTIFACT_EDITOR_ANY_ULTIMATE) + 1,
    SPHINX_ANSWER_LENGTH  = 11,
    SPHINX_ANSWER_BUFFER  = 100,
    SPHINX_RESOURCE_TEXT_SIZE = 52,
H2_ENUM_END(SphinxDialog)

DATA(0x004a5780) mapEventExtra gSphinx;
DATA(0x004a580c) char* gSphinxText;

VA(0x00424ef0, 0x308)
i32 eventsManager::EditSphinx(i32 extra) {
    i32 unused[2];
    tag_message message;
    i32 i;
    i32 len;
    char* newRecord;

    ResetPlayerAvailability();
    memcpy(&gSphinx, gEditManager->m_extras[extra], sizeof(gSphinx));
    gSphinxText = new char[EVENT_TEXT_CAPACITY];
    strcpy(gSphinxText, static_cast<mapEventExtra*>(gEditManager->m_extras[extra])->riddle);
    gEditDialog = new heroWindow(0, 0, "ridledit.bin");
    SetWinText(gEditDialog, SPHINX_WINDOW_TEXT_ID);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
    sprintf(gText, localization::Tr("editor.sphinx.no_artifact"));
    message.payload.widget.data.text = gText;
    message.payload.widget.id = SPHINX_ARTIFACT_LIST;
    gEditDialog->BroadcastMessage(message);
    for (i = 0; i < IDX(ARTIFACT_COUNT); i++) {
        if (i < IDX(ARTIFACT_EDITOR_ANY_ULTIMATE) || i > IDX(ARTIFACT_SPELL_SCROLL)) {
            sprintf(gText, "%s", gArtifactNames[i]);
            message.payload.widget.data.text = gText;
            gEditDialog->BroadcastMessage(message);
        }
    }
    for (i = 0; i < gSphinx.answerCount; i++) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.id = SPHINX_ANSWER_LIST;
        message.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
        message.payload.widget.data.text = gSphinx.answers[i];
        gEditDialog->BroadcastMessage(message);
    }
    FillInSphinxEdit(&gSphinx);
    gpWindowManager->DoDialog(gEditDialog, EditSphinxHandler, 0);
    delete gEditDialog;
    if (gpWindowManager->m_dialogResult != EVENTS_DIALOG_CANCEL) {
        len = strlen(gSphinxText) + sizeof(gSphinx);
        newRecord = new char[len];
        memcpy(newRecord, &gSphinx, sizeof(gSphinx));
        strcpy(newRecord + offsetof(mapEventExtra, riddle), gSphinxText);
        delete[] static_cast<char*>(gEditManager->m_extras[extra]);
        gEditManager->m_extras[extra] = newRecord;
        gEditManager->m_extraSizes[extra] = len;
        delete[] gSphinxText;
        gSphinxText = NULL;
        gEditManager->m_mapChanged = 1;
    }
    gEditManager->UpdateMapView();
    return gpWindowManager->m_dialogResult;
}

VA(0x004251f8, 0x170)
void eventsManager::FillInSphinxEdit(mapEventExtra* sphinx) {
    char text[SPHINX_RESOURCE_TEXT_SIZE];
    b32 dimmed;
    tag_message message;
    i32 i;

    message.type = MESSAGE_WIDGET;
    message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.data.text = text;
    for (i = 0; i < IDX(RES_COUNT); i++) {
        sprintf(text, "%d", gSphinx.resources[i]);
        message.payload.widget.id = SPHINX_RESOURCE_FIRST + i;
        gEditDialog->BroadcastMessage(message);
    }
    message.payload.widget.data.text = gSphinxText;
    message.payload.widget.id = EVENT_TEXT_FIELD;
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
    message.payload.widget.data.value = gSphinx.artifact + 1;
    if (gSphinx.artifact >= IDX(ARTIFACT_EDITOR_ANY_ULTIMATE))
        message.payload.widget.data.value -= SPHINX_SKIPPED_ARTIFACTS;
    message.payload.widget.id = SPHINX_ARTIFACT_LIST;
    gEditDialog->BroadcastMessage(message);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
    message.payload.widget.id = SPHINX_ANSWER_LIST;
    gEditDialog->BroadcastMessage(message);
    if (message.payload.widget.data.value == -1 && gSphinx.answerCount > 0) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
        message.payload.widget.id = SPHINX_ANSWER_LIST;
        message.payload.widget.data.value = 0;
        gEditDialog->BroadcastMessage(message);
    }
    dimmed = message.payload.widget.data.value == -1;
    message.payload.widget.command = dimmed ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.data.value = WIDGET_FLAGS_ARGUMENT_DIMMED;
    message.payload.widget.id = SPHINX_DELETE_ANSWER;
    gEditDialog->BroadcastMessage(message);
}

VA(0x00425368, 0x475)
MessageDispatchResult EditSphinxHandler(struct tag_message& message) {
    i32 unused[2];
    tag_message request;
    b32 modified;
    i32 unusedIndex;
    char newAnswer[SPHINX_ANSWER_BUFFER];
    i32 answerIndex;

    modified = false;
    SET_WIDGET_MESSAGE(request, WIDGET_COMMAND_GET_TEXT, message.payload.widget.id);
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
                        case SPHINX_ADD_ANSWER:
                            newAnswer[0] = '\0';
                            if (gSphinx.answerCount >= EVENT_RECORD_MAP_ANSWER_COUNT) {
                                NormalDialog(localization::Tr("editor.sphinx.answers_full"), NORMAL_DIALOG_INFO);
                            } else {
                                GetDataEntry(
                                    localization::Tr("editor.sphinx.enter_answer"),
                                    newAnswer, SPHINX_ANSWER_LENGTH, NULL, 0, 1);
                                strcpy(gSphinx.answers[gSphinx.answerCount], newAnswer);
                                gSphinx.answerCount++;
                                request.type = MESSAGE_WIDGET;
                                request.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
                                request.payload.widget.data.text = newAnswer;
                                request.payload.widget.id = SPHINX_ANSWER_LIST;
                                gEditDialog->BroadcastMessage(request);
                                modified = true;
                            }
                            break;
                        case SPHINX_DELETE_ANSWER:
                            request.type = MESSAGE_WIDGET;
                            request.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                            request.payload.widget.id = SPHINX_ANSWER_LIST;
                            gEditDialog->BroadcastMessage(request);
                            answerIndex = request.payload.widget.data.value;
                            if (answerIndex != -1) {
                                request.payload.widget.command = WIDGET_COMMAND_DELETE_ITEM;
                                request.payload.widget.data.value = answerIndex;
                                gEditDialog->BroadcastMessage(request);
                                modified = true;
                            }
                            break;
                    }
                    break;
                case WIDGET_NOTIFY_SELECT:
                    modified = true;
                    switch (message.payload.widget.id) {
                        case EVENT_TEXT_FIELD:
                            gEditDialog->BroadcastMessage(request);
                            strcpy(gSphinxText, request.payload.widget.data.text);
                            break;
                        case SPHINX_ARTIFACT_LIST:
                            message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                            gEditDialog->BroadcastMessage(message);
                            if (message.payload.widget.data.value - 1 >= IDX(ARTIFACT_EDITOR_ANY_ULTIMATE))
                                message.payload.widget.data.value += SPHINX_SKIPPED_ARTIFACTS;
                            gSphinx.artifact = message.payload.widget.data.value - 1;
                            modified = true;
                            break;
                        case SPHINX_RESOURCE_FIRST:
                        case SPHINX_RESOURCE_FIRST + 1:
                        case SPHINX_RESOURCE_FIRST + 2:
                        case SPHINX_RESOURCE_FIRST + 3:
                        case SPHINX_RESOURCE_FIRST + 4:
                        case SPHINX_RESOURCE_FIRST + 5:
                        case SPHINX_RESOURCE_LAST:
                            gEditDialog->BroadcastMessage(request);
                            gSphinx.resources[message.payload.widget.id - SPHINX_RESOURCE_FIRST]
                                = atoi(request.payload.widget.data.text);
                            break;
                        default:
                            modified = false;
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
    if (modified) {
        static_cast<eventsManager*>(gEditManager->m_toolManager)->FillInSphinxEdit(&gSphinx);
        gEditDialog->DrawWindow();
    }
    return MESSAGE_DISPATCH_CONSUME;
}
