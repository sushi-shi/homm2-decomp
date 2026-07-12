#ifndef HOMM2_SOURCE_HEXCELL_H
#define HOMM2_SOURCE_HEXCELL_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 8 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <BASE/icon.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class hexcell {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    short  field_0x0;  // +0x00
    short  field_0x2;  // +0x02
    char _pad_0x4[0xa];
    char   field_0xe;  // +0x0e
    signed char obstacleIndex;  // +0x0f
    signed char occupantSide;  // +0x10
    signed char occupantIndex;  // +0x11
    signed char occupantFrame;  // +0x12
    int deadOccupantCount;  // +0x13
    signed char deadOccupantSides[14];  // +0x17
    signed char deadOccupantIndices[14];  // +0x25
    signed char deadOccupantFrames[14];  // +0x33
    char field_0x41;
    struct SLimitData limits;  // +0x42
    // --- constructors ---
    hexcell(void);
    // --- methods ---
    void DrawGround(void);
    void DrawLowerDeadOccupants(void);
    void DrawUpperDeadOccupant(void);
    void DrawOccupant(int, int);
    void DrawTower(int);
    void DrawClouds(void);
    void DrawObstacle(void);
};
#pragma pack(pop)
SIZE(hexcell, 0x62);
#endif // HOMM2_SOURCE_HEXCELL_H
