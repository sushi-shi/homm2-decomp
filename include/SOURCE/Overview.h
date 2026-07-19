#ifndef HOMM2_OVERVIEW_H
#define HOMM2_OVERVIEW_H

#include <Ints.h>

struct tag_message;
class iconWidget;
class textWidget;

H2_ENUM_CLASS_BEGIN(OverviewType)
    OVERVIEW_NONE   = -1,
    OVERVIEW_HEROES = 0,
    OVERVIEW_TOWNS  = 1
H2_ENUM_CLASS_END(OverviewType)

H2_ENUM_BEGIN(OverviewLayoutConstant)
    OVERVIEW_VISIBLE_ROWS            = 4,
    OVERVIEW_DYNAMIC_WIDGETS_PER_ROW = 70,
    OVERVIEW_ROW_HEIGHT              = 86,
    OVERVIEW_ROW_ID_STRIDE           = 200,
    OVERVIEW_TROOP_SLOTS             = 5,
    OVERVIEW_DWELLING_SLOTS          = 12,
    OVERVIEW_SECONDARY_SKILL_SLOTS   = 8,
    OVERVIEW_ARTIFACT_SLOTS          = 14,
    OVERVIEW_SECONDARY_SKILL_COLUMNS = 4,
    OVERVIEW_ARTIFACT_COLUMNS        = 7
H2_ENUM_END(OverviewLayoutConstant)

typedef iconWidget* OverviewIconWidgetRow[OVERVIEW_DYNAMIC_WIDGETS_PER_ROW];
typedef textWidget* OverviewTextWidgetRow[OVERVIEW_DYNAMIC_WIDGETS_PER_ROW];

i32 OverviewHandler(struct tag_message&);

#endif
