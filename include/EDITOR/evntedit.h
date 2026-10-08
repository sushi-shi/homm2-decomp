#ifndef HOMM2_EDITOR_EVNTEDIT_H
#define HOMM2_EDITOR_EVNTEDIT_H


#include <Domains.h>
#include <BASE/message.h>
#include <SOURCE/gameTypes.h>

typedef enum EventEditConstant {

    EVENT_EDIT_FIRST_RESOURCE    = 200,
    EVENT_EDIT_LAST_RESOURCE     = 206,
    EVENT_EDIT_ARTIFACT          = 301,
    EVENT_EDIT_CANCEL_AFTER_VISIT = 304,
    EVENT_EDIT_APPLY_TO_COMPUTER = 307,
    EVENT_EDIT_FIRST_DAY         = 410,
    EVENT_EDIT_FREQUENCY         = 430,
    EVENT_EDIT_APPLY_TO_HUMAN    = 442,

    EVENT_EDIT_FIRST_PLAYER      = 460,
    EVENT_EDIT_FIRST_PLAYER_TOGGLE = 470,
    EVENT_EDIT_LAST_PLAYER_TOGGLE  = 475,


    EVENT_EDIT_FIRST_MAP_ROW     = 300,
    EVENT_EDIT_MAP_ROW_END       = 304,
    EVENT_EDIT_FIRST_TIME_ROW    = 400,
    EVENT_EDIT_TIME_ROW_END      = 479,


    EVENT_EDIT_DAILY_LAST        = 7,
    EVENT_EDIT_WEEKLY_BASE       = 6
} EventEditConstant;


extern EventExtra gEventEdit;
extern char* gEventMessage;

MessageDispatchResult EditEventHandler(struct tag_message& message);

#endif
