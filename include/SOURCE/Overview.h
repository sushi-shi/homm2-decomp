#ifndef HOMM2_OVERVIEW_H
#define HOMM2_OVERVIEW_H
// Declarations of the free functions DEFINED in Overview.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct tag_message;

typedef enum OverviewType {
    OVERVIEW_HEROES = 0,
    OVERVIEW_TOWNS = 1
} OverviewType;

typedef enum OverviewLayoutConstant {
    OVERVIEW_VISIBLE_ROWS = 4,
    OVERVIEW_DYNAMIC_WIDGETS_PER_ROW = 70,
    OVERVIEW_ROW_HEIGHT = 86,
    OVERVIEW_ROW_ID_STRIDE = 200,
    OVERVIEW_TROOP_SLOTS = 5,
    OVERVIEW_DWELLING_SLOTS = 12,
    OVERVIEW_SECONDARY_SKILL_SLOTS = 8,
    OVERVIEW_ARTIFACT_SLOTS = 14,
    OVERVIEW_SECONDARY_SKILL_COLUMNS = 4,
    OVERVIEW_ARTIFACT_COLUMNS = 7
} OverviewLayoutConstant;

int OverviewHandler(struct tag_message &);

#endif // HOMM2_OVERVIEW_H
