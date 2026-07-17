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
    i16  m_x;  // +0x00
    i16  m_y;  // +0x02
    i16 m_gridLeft;  // +0x04
    i16 m_gridTop;  // +0x06
    i16 m_gridRight;  // +0x08
    i16 m_gridBodyBottom;  // +0x0a
    i16 m_gridBottom;  // +0x0c
    u8 m_blocked;  // +0x0e
    i8 m_obstacleIndex;  // +0x0f
    i8 m_occupantSide;  // +0x10
    i8 m_occupantIndex;  // +0x11
    i8 m_occupantFrame;  // +0x12
    i32 m_deadOccupantCount;  // +0x13
    i8 m_deadOccupantSides[14];  // +0x17
    i8 m_deadOccupantIndices[14];  // +0x25
    i8 m_deadOccupantFrames[14];  // +0x33
    u8 m_pathReachable;  // +0x41
    struct SLimitData m_limits[2];  // +0x42
    // --- constructors ---
    hexcell(void);
    // --- methods ---
    void DrawGround(void);
    void DrawLowerDeadOccupants(void);
    void DrawUpperDeadOccupant(void);
    void DrawOccupant(i32, i32);
    void DrawTower(i32);
    void DrawClouds(void);
    void DrawObstacle(void);
};
#pragma pack(pop)
SIZE(hexcell, 0x62);
#endif // HOMM2_SOURCE_HEXCELL_H
