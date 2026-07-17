#ifndef HOMM2_VIEW_H
#define HOMM2_VIEW_H

#include <Ints.h>
// Declarations of the free functions DEFINED in VIEW.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct tag_message;

HOMM2_ENUM_VALUES_BEGIN(ViewGeneralConstant)
    VIEW_GENERAL_WINDOW_X = 179,
    VIEW_GENERAL_WINDOW_Y = 60,
    VIEW_GENERAL_SET_TEXT = 3,
    VIEW_GENERAL_SET_FRAME = 4,
    VIEW_GENERAL_SET_ICON = 9,
    VIEW_GENERAL_CLOSE = 10,
    VIEW_GENERAL_RETREAT = 11,
    VIEW_GENERAL_SURRENDER = 12,
    VIEW_GENERAL_CAST_SPELL = 0x7800,
    VIEW_GENERAL_TEXT_COLOR_COUNT = 11,
    VIEW_GENERAL_MORALE_TEXT_OFFSET = 3,
    VIEW_GENERAL_LUCK_TEXT_OFFSET = 3,
    VIEW_GENERAL_MANA_PER_KNOWLEDGE = 10,
    VIEW_ARMY_WIDTH = 488,
    VIEW_ARMY_HEIGHT = 229,
    VIEW_ARMY_SCREEN_WIDTH = 640,
    VIEW_ARMY_SCREEN_HEIGHT = 460,
    VIEW_ARMY_LEFT_FACING_X_OFFSET = 123,
    VIEW_ARMY_RIGHT_FACING_X_OFFSET = 80,
    VIEW_ARMY_FACING_OFFSET_DELTA = 43,
    VIEW_ARMY_Y_OFFSET = 164,
    VIEW_ARMY_RIGHT_CLAMP = 151,
    VIEW_ARMY_BOTTOM_CLAMP = 230
HOMM2_ENUM_VALUES_END(ViewGeneralConstant)

i32 HandleViewGeneral(struct tag_message&);
extern i32 iViewGeneralWhichSide;

#endif // HOMM2_VIEW_H
