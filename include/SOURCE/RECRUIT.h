#ifndef HOMM2_RECRUIT_H
#define HOMM2_RECRUIT_H

#include <Ints.h>
#include <SOURCE/KB_TYPES.h>

class heroWindow;
class town;

void SetupRecruitWin(
    class heroWindow*,
    CreatureType,
    i32,
    ResourceType,
    i32,
    i32
);
void QuickViewRecruit(class town*, i32);

#endif
