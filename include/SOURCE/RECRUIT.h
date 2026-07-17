#ifndef HOMM2_RECRUIT_H
#define HOMM2_RECRUIT_H

#include <Ints.h>
// Declarations of the free functions DEFINED in RECRUIT.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class heroWindow;
class town;

typedef enum RecruitConstant {
    RECRUIT_RESOURCE_COUNT = 6,
    RECRUIT_GOLD_RESOURCE = 6,
    RECRUIT_NO_RESOURCE = -1,
    RECRUIT_SOURCE_EVENT = -1,
    RECRUIT_SOURCE_TOWN = 0x23,
    RECRUIT_WINDOW_X = 0x8f,
    RECRUIT_WINDOW_Y = 0x10,
    RECRUIT_QUICK_WINDOW_X = 0xa0,
    RECRUIT_QUICK_WINDOW_Y = 0x10,
    RECRUIT_NAME_SIZE = 20,
    RECRUIT_LABEL_SIZE = 40,
    RECRUIT_MANAGER_MASK = 0x4000,
    RECRUIT_BROADCAST_FLAGS = 0x4008,
    RECRUIT_DRAW_DEPTH = 0x7fff,
    RECRUIT_VIEW_ARMY_X = 0x77,
    RECRUIT_VIEW_ARMY_Y = 0x20
} RecruitConstant;

typedef enum RecruitControl {
    RECRUIT_TITLE_CONTROL = 0x40,
    RECRUIT_CREATURE_CONTROL = 0x42,
    RECRUIT_AVAILABLE_CONTROL = 0x43,
    RECRUIT_QUANTITY_CONTROL = 0x44,
    RECRUIT_INCREASE_CONTROL = 0x45,
    RECRUIT_DECREASE_CONTROL = 0x46,
    RECRUIT_MAXIMUM_CONTROL = 0x47,
    RECRUIT_GOLD_ICON_CONTROL = 0x49,
    RECRUIT_RESOURCE_ICON_CONTROL = 0x4a,
    RECRUIT_RESOURCE_COST_CONTROL = 0x4b,
    RECRUIT_GOLD_TOTAL_CONTROL = 0x4d,
    RECRUIT_RESOURCE_IMAGE_CONTROL = 0x4e,
    RECRUIT_RESOURCE_TOTAL_CONTROL = 0x4f,
    RECRUIT_CLOSE_CONTROL = 0x7800,
    RECRUIT_CANCEL_CONTROL = 0x7801,
    RECRUIT_CONFIRM_CONTROL = 0x7802
} RecruitControl;

void SetupRecruitWin(class heroWindow *, i32, i32, i32, i32, i32);
void QuickViewRecruit(class town *, i32);

#endif // HOMM2_RECRUIT_H
