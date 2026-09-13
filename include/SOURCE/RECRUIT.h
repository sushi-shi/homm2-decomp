#ifndef HOMM2_RECRUIT_H
#define HOMM2_RECRUIT_H

#include <Ints.h>
#include <SOURCE/KB_TYPES.h>

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
