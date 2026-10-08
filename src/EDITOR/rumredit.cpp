// The rumour editor: the map specification dialog opens it for one of the
// map's rumours. The unit name comes from its dialog resource
// (rumredit.bin); descriptive names: EditRumour, EditRumourHandler,
// gRumour, gRumourText. FillInRumourEdit takes the name of the Price of
// Loyalty editor's FillInEventEdit family.

#include <match.h>
#include <EDITOR/rumredit.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/editManager.h>
#include <EDITOR/eventsManager.h>
#include <BASE/dialog.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/message.h>
#include <SOURCE/KB.h>
#include <stddef.h>
#include <string.h>

DATA(0x004a5810) rumourEventExtra gRumour;
DATA(0x004a581c) char* gRumourText;

VA(0x00425820, 0x20c)
i32 eventsManager::EditRumour(i32 extra) {
    i32 H2_UNUSED(unused)[3];
    char* newRecord;
    i32 len;

    memcpy(&gRumour, gEditManager->m_extras[extra], sizeof(gRumour));
    gRumourText = new char[EVENT_TEXT_CAPACITY];
    strcpy(gRumourText, static_cast<rumourEventExtra*>(gEditManager->m_extras[extra])->text);
    gEditDialog = new heroWindow(0, 0, "rumredit.bin");
    SetWinText(gEditDialog, EDITOR_WIN_TEXT_RUMOUR);
    FillInRumourEdit(&gRumour);
    gpWindowManager->DoDialog(gEditDialog, EditRumourHandler, 0);
    delete gEditDialog;
    if (gpWindowManager->m_dialogResult != EVENTS_DIALOG_CANCEL) {
        len = strlen(gRumourText) + sizeof(gRumour);
        newRecord = new char[len];
        memcpy(newRecord, &gRumour, sizeof(gRumour));
        strcpy(newRecord + offsetof(rumourEventExtra, text), gRumourText);
        delete[] static_cast<char*>(gEditManager->m_extras[extra]);
        gEditManager->m_extras[extra] = newRecord;
        gEditManager->m_extraSizes[extra] = len;
        delete[] gRumourText;
        gRumourText = NULL;
        gEditManager->m_mapChanged = true;
    }
    gEditManager->UpdateMapView();
    return gpWindowManager->m_dialogResult;
}

VA(0x00425a2c, 0x3b)
void eventsManager::FillInRumourEdit(rumourEventExtra* H2_UNUSED(rumour)) {
    tag_message message;

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.data.text = gRumourText;
    message.payload.widget.id = EVENT_TEXT_FIELD;
    gEditDialog->BroadcastMessage(message);
}

VA(0x00425a67, 0x14d)
MessageDispatchResult EditRumourHandler(struct tag_message& message) {
    i32 H2_UNUSED(unused)[2];
    b32 modified = false;
    tag_message reply;
    i32 H2_UNUSED(unusedIndex);

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
                            strcpy(gRumourText, reply.payload.widget.data.text);
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
        static_cast<eventsManager*>(gEditManager->m_toolManager)->FillInRumourEdit(&gRumour);
        gEditDialog->DrawWindow();
    }
    return MESSAGE_DISPATCH_CONSUME;
}
