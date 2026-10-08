#ifndef HOMM2_RECRUIT_H
#define HOMM2_RECRUIT_H

#include <Domains.h>
#include <SOURCE/kbTypes.h>

class heroWindow;
class town;

void SetupRecruitWin(
    class heroWindow* window,
    H2_ENUM_PARAM(CreatureType, i32) creatureType,
    i32 goldCost,
    ResourceType resourceType,
    i32 resourceCost,
    i32 available
);
void QuickViewRecruit(class town* townData, i32 dwelling);

#endif
