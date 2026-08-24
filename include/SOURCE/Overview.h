#ifndef HOMM2_OVERVIEW_H
#define HOMM2_OVERVIEW_H

#include <Ints.h>
#include <BASE/message.h>

struct tag_message;
class iconWidget;
class textWidget;

enum {
    OVERVIEW_NONE       = -1,
    OVERVIEW_HEROES     = 0,
    OVERVIEW_TOWNS      = 1,
    OVERVIEW_TYPE_COUNT = 2
};
typedef i32 OverviewType;
enum {
    OVERVIEW_RETURN_NONE = -1,
    OVERVIEW_RETURN_HERO = 1,
    OVERVIEW_RETURN_TOWN = 2
};
typedef i32 OverviewReturnAction;
typedef enum OverviewLayoutConstant {
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
} OverviewLayoutConstant;

typedef iconWidget* OverviewIconWidgetRow[OVERVIEW_DYNAMIC_WIDGETS_PER_ROW];
typedef textWidget* OverviewTextWidgetRow[OVERVIEW_DYNAMIC_WIDGETS_PER_ROW];

MessageDispatchResult OverviewHandler(struct tag_message&);

#endif
