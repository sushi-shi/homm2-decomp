#ifndef HOMM2_EDITOR_EVENTSMANAGER_H
#define HOMM2_EDITOR_EVENTSMANAGER_H


#include <Ints.h>
#include <BASE/baseManager.h>
#include <BASE/dialog.h>
#include <BASE/message.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/EVENTMGR.h>
#include <SOURCE/KB_TYPES.h>

class icon;
struct mapEventExtra;
struct rumourEventExtra;
struct signEventExtra;

typedef enum EventsDialogButton {


    EVENTS_DIALOG_CANCEL = DIALOG_BUTTON_1,
    EVENTS_DIALOG_OK     = DIALOG_BUTTON_2
} EventsDialogButton;

typedef enum EventsArtifactList {


    EVENTS_HIDDEN_ARTIFACT_COUNT =
        H2EnumIndex(ARTIFACT_SPELL_SCROLL) - H2EnumIndex(ARTIFACT_EDITOR_ANY_ULTIMATE) + 1
} EventsArtifactList;

typedef enum EventTextConstant {


    EVENT_TEXT_CAPACITY = 2000,
    EVENT_TEXT_FIELD    = 0x78,

    EVENTS_NUMBER_TEXT_SIZE = 20,
    EVENTS_FIELD_TEXT_SIZE  = 50
} EventTextConstant;


#define FINISH_EDIT_DIALOG(message)                                                                \
    ((message).type = MESSAGE_WIDGET,                                                              \
     (message).payload.widget.id = H2EnumIndex(WIDGET_COMMAND_DIALOG_SELECT),                              \
     (message).payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT)

#pragma pack(push, 1)
class eventsManager : public baseManager {
public:

    icon* m_overlayIcon;

    eventsManager(void);
    virtual i32 Open(i32 priority) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message& message) override;

    void EditCell(i32 x, i32 y);

    void EditMonster(i32 x, i32 y, b32 ultimateArtifact);

    i32 EditEvent(i32 extra);
    void FillInEventEdit(struct EventExtra* event);

    void EditHero(i32 x, i32 y, b32 jailed);
    void FillInHeroEdit(struct HeroExtra* hero);

    void EditTown(i32 x, i32 y);
    void FillInTownEdit(struct TownExtra* town);

    i32 EditSphinx(i32 extra);
    void FillInSphinxEdit(mapEventExtra* sphinx);

    i32 EditRumour(i32 extra);
    void FillInRumourEdit(rumourEventExtra* rumour);

    void EditSign(i32 x, i32 y);
    void FillInSignEdit(signEventExtra* sign);

    void EditSpellScroll(i32* spell);
};
#pragma pack(pop)

#endif
