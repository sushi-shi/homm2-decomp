#ifndef HOMM2_SOURCE_HEXCELL_H
#define HOMM2_SOURCE_HEXCELL_H

#include <va.h>
#include <BASE/icon.h>

#define COMBAT_OCCUPANT_NONE (-1)
#define COMBAT_DEAD_OCCUPANT_CAPACITY 14
#define HEXCELL_LIMIT_COUNT 2

#pragma pack(push, 1)
class hexcell {
public:
    i16 m_x;
    i16 m_y;
    i16 m_gridLeft;
    i16 m_gridTop;
    i16 m_gridRight;
    i16 m_gridBodyBottom;
    i16 m_gridBottom;
    u8 m_blocked;
    i8 m_obstacleIndex;
    i8 m_occupantSide;
    i8 m_occupantIndex;
    i8 m_occupantFrame;
    i32 m_deadOccupantCount;
    i8 m_deadOccupantSides[COMBAT_DEAD_OCCUPANT_CAPACITY];
    i8 m_deadOccupantIndices[COMBAT_DEAD_OCCUPANT_CAPACITY];
    i8 m_deadOccupantFrames[COMBAT_DEAD_OCCUPANT_CAPACITY];
    u8 m_pathReachable;
    struct SLimitData m_limits[HEXCELL_LIMIT_COUNT];
    hexcell(void);
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
#endif
