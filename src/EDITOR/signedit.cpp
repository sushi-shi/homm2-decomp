

#include <Ints.h>
#include <EDITOR/signedit.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/editManager.h>
#include <EDITOR/eventsManager.h>
#include <EDITOR/fullMap.h>
#include <EDITOR/mapcell.h>
#include <BASE/dialog.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/message.h>
#include <SOURCE/KB.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef enum SignDialog {
    SIGN_TITLE          = 0x64,
} SignDialog;

signEventExtra gSign;
char* gSignText;

void eventsManager::EditSign(i32 x, i32 y) {
    mapCell original;
    i32 unused [[maybe_unused]][3];
    tag_message message;
    i32 len;
    char* newRecord;

    gEditCell = gMap.GetCell(x, y);
    if (gEditCell->m_objectMetadata == 0) {
        NormalDialog("Нельзя редактировать указатель. Объект создан старым редактором.", NORMAL_DIALOG_INFO);
        return;
    }
    original = *gEditCell;
    memcpy(&gSign, gEditManager->m_extras[gEditCell->m_objectMetadata], sizeof(gSign));
    gSignText = new char[EVENT_TEXT_CAPACITY];
    strcpy(gSignText, static_cast<signEventExtra*>(gEditManager->m_extras[gEditCell->m_objectMetadata])->text);
    gEditDialog = new heroWindow(0, 0, "rumredit.bin");
    SetWinText(gEditDialog, EDITOR_WIN_TEXT_RUMOUR);
    if (gEditCell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_BOTTLE))
        sprintf(gText, "Содержание бутылки");
    else
        sprintf(gText, "Информация указателя");
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.data.text = gText;
    message.payload.widget.id = SIGN_TITLE;
    gEditDialog->BroadcastMessage(message);
    FillInSignEdit(&gSign);
    gpWindowManager->DoDialog(gEditDialog, EditSignHandler, 0);
    delete gEditDialog;
    if (gpWindowManager->m_dialogResult != EVENTS_DIALOG_CANCEL) {
        len = strlen(gSignText) + sizeof(gSign);
        if (!gSign.active)
            strcpy(gSignText, "");
        newRecord = new char[len];
        memcpy(newRecord, &gSign, sizeof(gSign));
        strcpy(newRecord + offsetof(signEventExtra, text), gSignText);
        delete[] static_cast<char*>(gEditManager->m_extras[gEditCell->m_objectMetadata]);
        gEditManager->m_extras[gEditCell->m_objectMetadata] = newRecord;
        gEditManager->m_extraSizes[gEditCell->m_objectMetadata] = len;
        delete[] gSignText;
        gSignText = NULL;
        gEditManager->m_mapChanged = true;
    }
    gEditManager->DrawMap();
    gEditManager->UpdateMapView();
}

void eventsManager::FillInSignEdit(signEventExtra* sign [[maybe_unused]]) {
    i32 unused [[maybe_unused]];
    tag_message message;

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.data.text = gSignText;
    message.payload.widget.id = EVENT_TEXT_FIELD;
    gEditDialog->BroadcastMessage(message);
}

MessageDispatchResult EditSignHandler(struct tag_message& message) {
    i32 unused [[maybe_unused]][2];
    b32 modified = false;
    tag_message reply;
    i32 unusedIndex [[maybe_unused]];

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
                        case EVENT_TEXT_FIELD:
                            SET_WIDGET_MESSAGE(reply, WIDGET_COMMAND_GET_TEXT, message.payload.widget.id);
                            gEditDialog->BroadcastMessage(reply);
                            strcpy(gSignText, reply.payload.widget.data.text);
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
        static_cast<eventsManager*>(gEditManager->m_toolManager)->FillInSignEdit(&gSign);
        gEditDialog->DrawWindow();
    }
    return MESSAGE_DISPATCH_CONSUME;
}
