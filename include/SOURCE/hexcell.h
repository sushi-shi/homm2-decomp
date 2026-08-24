#ifndef HOMM2_SOURCE_HEXCELL_H
#define HOMM2_SOURCE_HEXCELL_H

#include <Ints.h>
#include <BASE/icon.h>
#include <SOURCE/combatTypes.h>

typedef enum HexcellConstant {
    COMBAT_DEAD_OCCUPANT_CAPACITY = 14,
    HEXCELL_LIMIT_COUNT           = 2
} HexcellConstant;

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
    H2EnumStorage<CombatSide, i8> m_occupantSide;
    i8 m_occupantIndex;
    H2EnumStorage<ArmyFacing, i8> m_occupantFrame;
    i32 m_deadOccupantCount;
    H2EnumStorage<CombatSide, i8> m_deadOccupantSides[COMBAT_DEAD_OCCUPANT_CAPACITY];
    i8 m_deadOccupantIndices[COMBAT_DEAD_OCCUPANT_CAPACITY];
    H2EnumStorage<ArmyFacing, i8> m_deadOccupantFrames[COMBAT_DEAD_OCCUPANT_CAPACITY];
    u8 m_pathReachable;
    struct SLimitData m_limits[HEXCELL_LIMIT_COUNT];
    hexcell(void);
    void DrawGround(void);
    void DrawLowerDeadOccupants(void);
    void DrawUpperDeadOccupant(void);
    void DrawOccupant(ArmyDrawState, i32);
    void DrawTower(i32);
    void DrawClouds(void);
    void DrawObstacle(void);
};
#pragma pack(pop)
#endif
