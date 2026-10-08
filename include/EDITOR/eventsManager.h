#ifndef HOMM2_EDITOR_EVENTSMANAGER_H
#define HOMM2_EDITOR_EVENTSMANAGER_H

// The detail tool (src/EDITOR/EVENTMGR.cpp): a click on a map cell opens the
// editor for the object there. The unit name EVENTMGR is descriptive; Open
// stores the class name "eventsManager". Each object's own dialog editor
// lives in its own unit (evntedit, heroedit, ridledit, rumredit, signedit,
// townedit, x_spedit); EVENTMGR keeps the raw cell editor, the monster and
// ultimate artifact editor and the random map generator's settings dialog
// (EVENTMGR.h). This header also holds the vocabulary every object dialog
// shares.

#include <match.h>
#include <Domains.h>
#include <H2/Macros.h>
#include <BASE/baseManager.h>
#include <BASE/dialog.h>
#include <BASE/message.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/EVENTMGR.h>
#include <SOURCE/kbTypes.h>

class icon;
struct mapEventExtra;
struct rumourEventExtra;
struct signEventExtra;

H2_ENUM_BEGIN(EventsDialogButton)
    // The editor dialogs' closing buttons (heroWindowManager::m_dialogResult):
    // cancel keeps the edited cell or record as it was.
    EVENTS_DIALOG_CANCEL = DIALOG_BUTTON_1,
    EVENTS_DIALOG_OK     = DIALOG_BUTTON_2
H2_ENUM_END(EventsDialogButton)

H2_ENUM_BEGIN(EventsArtifactList)
    // The dialogs' artifact lists leave out the editor-only artifacts from
    // ARTIFACT_EDITOR_ANY_ULTIMATE through the spell scroll.
    EVENTS_HIDDEN_ARTIFACT_COUNT =
        IDX(ARTIFACT_SPELL_SCROLL) - IDX(ARTIFACT_EDITOR_ANY_ULTIMATE) + 1
H2_ENUM_END(EventsArtifactList)

H2_ENUM_BEGIN(EventTextConstant)
    // The buffer a record's message is edited in, and the dialog's text
    // field (evntedit.bin, ridledit.bin, rumredit.bin).
    EVENT_TEXT_CAPACITY = 2000,
    EVENT_TEXT_FIELD    = 0x78,
    // The buffers the dialogs format a number or a field's text in.
    EVENTS_NUMBER_TEXT_SIZE = 20,
    EVENTS_FIELD_TEXT_SIZE  = 50
H2_ENUM_END(EventTextConstant)

// Closes the running dialog: the dialog manager reads the select command.
#define FINISH_EDIT_DIALOG(message)                                                                \
    ((message).type = MESSAGE_WIDGET,                                                              \
     (message).payload.widget.id = IDX(WIDGET_COMMAND_DIALOG_SELECT),                              \
     (message).payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT)

#pragma pack(push, 1)
class eventsManager : public baseManager {
public:
    // The outline drawn over the hovered cell (overlay.icn).
    icon* m_overlayIcon;

    eventsManager(void);
    virtual i32 Open(i32 priority) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual MessageDispatchResult Main(struct tag_message& message) OVERRIDE;
    // The raw cell editor (debug level 1 and above).
    void EditCell(i32 x, i32 y);
    // A monster's count, or the ultimate artifact's dig radius.
    void EditMonster(i32 x, i32 y, b32 ultimateArtifact);
    // evntedit: a map event.
    i32 EditEvent(i32 extra);
    void FillInEventEdit(struct EventExtra* event);
    // heroedit: a hero, or a jailed hero.
    void EditHero(i32 x, i32 y, b32 jailed);
    void FillInHeroEdit(struct HeroExtra* hero);
    // townedit: a town or castle.
    void EditTown(i32 x, i32 y);
    void FillInTownEdit(struct TownExtra* town);
    // ridledit: the sphinx's riddle, answers and reward.
    i32 EditSphinx(i32 extra);
    void FillInSphinxEdit(mapEventExtra* sphinx);
    // rumredit: one of the map's rumours.
    i32 EditRumour(i32 extra);
    void FillInRumourEdit(rumourEventExtra* rumour);
    // signedit: a sign's or a bottle's message.
    void EditSign(i32 x, i32 y);
    void FillInSignEdit(signEventExtra* sign);
    // x_spedit: a spell scroll's spell.
    void EditSpellScroll(i32* spell);
};
#pragma pack(pop)
SIZE(eventsManager, 0x3a);

#endif
