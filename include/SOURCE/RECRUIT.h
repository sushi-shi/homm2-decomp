#ifndef HOMM2_RECRUIT_H
#define HOMM2_RECRUIT_H

#include <Ints.h>

class heroWindow;
class town;

void SetupRecruitWin(class heroWindow*, i32, i32, ResourceType, i32, i32);
void QuickViewRecruit(class town*, i32);

#endif
