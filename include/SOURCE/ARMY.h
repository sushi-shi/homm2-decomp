#ifndef HOMM2_ARMY_H
#define HOMM2_ARMY_H
// Declarations of the free functions DEFINED in ARMY.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

#pragma pack(push, 1)
struct SMonFrameInfo {
    char pad0[0x65];
    signed char walkXOffsets[0x3d];  // +0x65
    int walkDuration;                // +0xa2
    char padA6[0x53];
    signed char walkFrameCount;      // +0xf9
    char padFA[0x7b];
    signed char walkFrames[0x39];    // +0x175
    char pad1AE[0x18a];
};
SIZE(SMonFrameInfo, 0x338);
#pragma pack(pop)
extern SMonFrameInfo sViewArmyMonFrameInfo;

void BuildTempWalkSeq(struct SMonFrameInfo *, int, int);
void ModifyFrameInfo(struct SMonFrameInfo *, int);

#endif // HOMM2_ARMY_H
