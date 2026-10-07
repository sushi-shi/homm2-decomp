// The spell scroll editor: the event tool opens it for a spell scroll and
// lists every spell to choose from. The unit name comes from its dialog
// resource (x_spedit.bin); descriptive names: EditSpellScroll,
// EditSpellScrollHandler, gSpellScrollChoice.

#include <va.h>
#include <EDITOR/x_spedit.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/editManager.h>
#include <EDITOR/eventsManager.h>
#include <BASE/dialog.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/message.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <stdio.h>
#include <string.h>

H2_ENUM_BEGIN(SpellScrollDialog)
    SPELL_SCROLL_LIST   = 0x64,
    SPELL_SCROLL_TITLE  = 0x65,
    SPELL_SCROLL_PROMPT = 0x66,
H2_ENUM_END(SpellScrollDialog)

DATA(0x004a5ddc) i32 gSpellScrollChoice;

VA(0x0042d010, 0x1d0)
void eventsManager::EditSpellScroll(i32* spell) {
    tag_message message;
    i32 i;

    gSpellScrollChoice = *spell;
    gEditDialog = new heroWindow(0, 0, "x_spedit.bin");
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.data.text = gText;
    message.payload.widget.id = SPELL_SCROLL_TITLE;
    strcpy(gText, localization::Tr("editor.spell_scroll.title"));
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.id = SPELL_SCROLL_PROMPT;
    strcpy(gText, localization::Tr("editor.spell_scroll.prompt"));
    gEditDialog->BroadcastMessage(message);
    message.payload.widget.command = WIDGET_COMMAND_APPEND_ITEM;
    message.payload.widget.id = SPELL_SCROLL_LIST;
    for (i = 0; i < IDX(SPELL_COUNT); i++) {
        sprintf(gText, "%s", gSpellNames[i]);
        message.payload.widget.data.text = gText;
        gEditDialog->BroadcastMessage(message);
    }
    message.payload.widget.command = WIDGET_COMMAND_SET_SELECTION;
    message.payload.widget.data.value = *spell;
    gEditDialog->BroadcastMessage(message);
    gpWindowManager->DoDialog(gEditDialog, EditSpellScrollHandler, 0);
    delete gEditDialog;
    if (gpWindowManager->m_dialogResult != EVENTS_DIALOG_CANCEL) {
        *spell = gSpellScrollChoice;
        gEditManager->m_mapChanged = true;
    }
    gEditManager->DrawMap();
    gEditManager->UpdateMapView();
}

VA(0x0042d1e0, 0x118)
MessageDispatchResult EditSpellScrollHandler(struct tag_message& message) {
    b32 H2_UNUSED(handled) = false;

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
                        case SPELL_SCROLL_LIST:
                            message.payload.widget.command = WIDGET_COMMAND_GET_SELECTION;
                            gEditDialog->BroadcastMessage(message);
                            gSpellScrollChoice = message.payload.widget.data.value;
                            handled = true;
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
    return MESSAGE_DISPATCH_CONSUME;
}
