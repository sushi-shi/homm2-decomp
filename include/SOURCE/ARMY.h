#ifndef HOMM2_ARMY_H
#define HOMM2_ARMY_H
// Declarations of the free functions DEFINED in ARMY.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

#pragma pack(push, 1)
struct SMonFrameInfo { char data[0x338]; };
#pragma pack(pop)
extern SMonFrameInfo sViewArmyMonFrameInfo;

void BuildTempWalkSeq(struct SMonFrameInfo *, int, int);
void ModifyFrameInfo(struct SMonFrameInfo *, int);

#endif // HOMM2_ARMY_H
