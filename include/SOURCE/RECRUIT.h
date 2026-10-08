#ifndef HOMM2_SOURCE_RECRUIT_H
#define HOMM2_SOURCE_RECRUIT_H

#include <Domains.h>
#include <SOURCE/kbTypes.h>

class heroWindow;
class town;

void SetupRecruitWin(
    class heroWindow* window,
    CreatureType creatureType,
    i32 goldCost,
    ResourceType resourceType,
    i32 resourceCost,
    i32 available
);
void QuickViewRecruit(class town* townData, i32 dwelling);

#endif
