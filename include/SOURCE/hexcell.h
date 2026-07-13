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
    short  m_x;  // +0x00
    short  m_y;  // +0x02
    char _pad_0x4[0xa];
    char   m_blocked;  // +0x0e
    signed char m_obstacleIndex;  // +0x0f
    signed char m_occupantSide;  // +0x10
    signed char m_occupantIndex;  // +0x11
    signed char m_occupantFrame;  // +0x12
    int m_deadOccupantCount;  // +0x13
    signed char m_deadOccupantSides[14];  // +0x17
    signed char m_deadOccupantIndices[14];  // +0x25
    signed char m_deadOccupantFrames[14];  // +0x33
    char m_unknown41;
    struct SLimitData m_limits;  // +0x42
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
