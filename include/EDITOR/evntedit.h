#ifndef HOMM2_EDITOR_EVNTEDIT_H
#define HOMM2_EDITOR_EVNTEDIT_H

// The event editor (src/EDITOR/evntedit.cpp, evntedit.bin):
// eventsManager::EditEvent, FillInEventEdit and the dialog handler. The
// dialog shows a map event's rows (isMapEvent) or a time event's.

#include <Domains.h>
#include <BASE/message.h>
#include <SOURCE/GAME.h>

H2_ENUM_BEGIN(EventEditConstant)
    // evntedit.bin's controls; the message is EVENT_TEXT_FIELD.
    EVENT_EDIT_FIRST_RESOURCE    = 200,
    EVENT_EDIT_LAST_RESOURCE     = 206,
    EVENT_EDIT_ARTIFACT          = 301,
    EVENT_EDIT_CANCEL_AFTER_VISIT = 304,
    EVENT_EDIT_APPLY_TO_COMPUTER = 307,
    EVENT_EDIT_FIRST_DAY         = 410,
    EVENT_EDIT_FREQUENCY         = 430,
    EVENT_EDIT_APPLY_TO_HUMAN    = 442,
    // A player's availability (shown) and its event toggle.
    EVENT_EDIT_FIRST_PLAYER      = 460,
    EVENT_EDIT_FIRST_PLAYER_TOGGLE = 470,
    EVENT_EDIT_LAST_PLAYER_TOGGLE  = 475,
    // The map event's rows and the time event's rows: each set is shown
    // and the other hidden.
    EVENT_EDIT_FIRST_MAP_ROW     = 300,
    EVENT_EDIT_MAP_ROW_END       = 304,
    EVENT_EDIT_FIRST_TIME_ROW    = 400,
    EVENT_EDIT_TIME_ROW_END      = 479,
    // The frequency list: "never" and every 1..7 days, then every 14, 21
    // and 28 (a week count + EVENT_EDIT_WEEKLY_BASE).
    EVENT_EDIT_DAILY_LAST        = 7,
    EVENT_EDIT_WEEKLY_BASE       = 6
H2_ENUM_END(EventEditConstant)

// The event the open dialog edits, and its message.
extern EventExtra gEventEdit;
extern char* gEventMessage;

MessageDispatchResult EditEventHandler(struct tag_message& message);

#endif
