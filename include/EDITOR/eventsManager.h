#ifndef HOMM2_EDITOR_EVENTSMANAGER_H
#define HOMM2_EDITOR_EVENTSMANAGER_H

// The detail tool (src/EDITOR/EVENTMGR.cpp): a click on a map cell opens the
// editor for the object there. Each object's own dialog editor lives in its
// own unit (ridledit, rumredit, signedit, x_spedit). This copy declares only
// what those units use; the class layout belongs to EVENTMGR.

#include <va.h>
#include <BASE/baseManager.h>
#include <BASE/dialog.h>
#include <BASE/message.h>

struct mapEventExtra;
struct rumourEventExtra;
struct signEventExtra;
struct tag_message;

H2_ENUM_BEGIN(EventsDialogButton)
    // The editor dialogs' closing buttons (heroWindowManager::m_dialogResult):
    // cancel keeps the edited cell or record as it was.
    EVENTS_DIALOG_CANCEL = DIALOG_BUTTON_1,
    EVENTS_DIALOG_OK     = DIALOG_BUTTON_2
H2_ENUM_END(EventsDialogButton)

H2_ENUM_BEGIN(EventTextConstant)
    // The buffer a record's message is edited in, and the dialog's text
    // field (ridledit.bin, rumredit.bin).
    EVENT_TEXT_CAPACITY = 2000,
    EVENT_TEXT_FIELD    = 0x78
H2_ENUM_END(EventTextConstant)

// Closes the running dialog: the dialog manager reads the select command.
#define FINISH_EDIT_DIALOG(message)                                                                \
    ((message).type = MESSAGE_WIDGET,                                                              \
     (message).payload.widget.id = IDX(WIDGET_COMMAND_DIALOG_SELECT),                              \
     (message).payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT)

class eventsManager : public baseManager {
public:
    // evntedit: a map event or a time event.
    i32 EditEvent(i32 extra);
    // ridledit: the sphinx's riddle, answers and reward.
    i32 EditSphinx(i32 extra);
    void UpdateSphinx(mapEventExtra* sphinx);
    // rumredit: one of the map's rumours.
    i32 EditRumor(i32 extra);
    void UpdateRumor(rumourEventExtra* rumor);
    // signedit: a sign's or a bottle's message.
    void EditSign(i32 x, i32 y);
    void UpdateSign(signEventExtra* sign);
    // x_spedit: a spell scroll's spell.
    void EditSpellScroll(i32* spell);
};

#endif
