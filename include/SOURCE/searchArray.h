#ifndef HOMM2_SOURCE_SEARCHARRAY_H
#define HOMM2_SOURCE_SEARCHARRAY_H

#include <H2/Ints.h>
#include <Domains.h>
#include <SOURCE/combatTypes.h>

class army;
extern i32 MAP_WIDTH;

typedef enum SearchConstant {
    SEARCH_QUEUE_CAPACITY             = 1024,
    SEARCH_PATH_CAPACITY              = 256,
    SEARCH_FLAG_BIT_COUNT             = 1,
    SEARCH_DIRECTION_BIT_COUNT        = 4,
    SEARCH_CELL_PAD_SIZE              = 4,
    SEARCH_MAX_COST                   = 999999,
    SEARCH_TARGET_COST_WINDOW         = 75,
    SEARCH_MONSTER_RESEED_WINDOW      = 300,
    SEARCH_DIAGONAL_COST_MASK         = 1,
    SEARCH_INVALID_COORDINATE         = -1
} SearchConstant;

#pragma pack(push, 1)
struct searchCell {
    u16 cost;
    u16 previous;
    u8 flags : SEARCH_FLAG_BIT_COUNT;
    char pad[SEARCH_CELL_PAD_SIZE];
};
#pragma pack(pop)

struct searchStorage {
    union {
        struct searchCell* cells;
        struct searchNode* nodes;
    };
    u8 directions[SEARCH_PATH_CAPACITY];
};

#pragma pack(push, 1)
struct searchNode {
    u8 x;
    u8 y;
    u16 distance;
    u8 visited : SEARCH_FLAG_BIT_COUNT;
    u8 occupied : SEARCH_FLAG_BIT_COUNT;

    u8 hasAdjacentMonster : SEARCH_FLAG_BIT_COUNT;


    u8 beyondTurnMobility : SEARCH_FLAG_BIT_COUNT;
    u8 direction : SEARCH_DIRECTION_BIT_COUNT;
    union {
        struct {
            u8 adjacentMonsterX;
            u8 adjacentMonsterY;
            u8 turnEndX;
            u8 turnEndY;
        };
        struct {
            i8 signedAdjacentMonsterX;
            i8 signedAdjacentMonsterY;
            i8 signedTurnEndX;
            i8 signedTurnEndY;
        };
    };
};
#pragma pack(pop)

class searchArray {
public:
    u32 m_queueCount;
    u32 m_maxQueueCount;
    i32 m_pathLength;
    i32 m_specialTargetX;
    i32 m_specialTargetY;
    searchNode m_queue[SEARCH_QUEUE_CAPACITY];
    searchStorage m_storage;
    searchNode* GetRow(i32 y, i32 width) {
        return m_storage.nodes + y * width;
    }
    searchNode* GetColumn(i32 x) {
        return m_storage.nodes + x;
    }
    searchNode& GetNode(i32 x, i32 y) {
        return *(m_storage.nodes + x + MAP_WIDTH * y);
    }
    searchArray(void);
    ~searchArray();
    i32 BuildPath(i32 startX, i32 startY, i32 destinationX, i32 destinationY, i32 maximumCost);
    void SeedPosition(
        i32 seedX,
        i32 seedY,
        MapDirection seedDirection,
        i32 maximumCost,
        i32 waterMode,
        i32 findAdjacentMonster,
        i32 mobility,
        i32 pathfindingSkill,
        i32 targetX,
        i32 targetY,
        i32 continueSeed,
        i32 seedMonsterCells
    );
    void Init(void);
    void Close(void);
    void Clear(void);
    i32 QuickDistance(i32 x1, i32 y1, i32 x2, i32 y2);
    void PushPoint(
        i32 x,
        i32 y,
        MapDirection direction,
        i32 cost,
        i32 maximumCost,
        i32 occupied,
        i32 hasAdjacentMonster,
        i32 adjacentMonsterX,
        i32 adjacentMonsterY,
        i32 beyondTurnMobility,
        i32 turnEndX,
        i32 turnEndY
    );
    void TestPossibleDirections(
        i32 x,
        i32 y,
        H2EnumStorage<TerrainType, i8> * const terrain,
        i8* const occupied,
        i32 allowOccupied,
        i32 waterMode
    );
    void SeedCombatPosition(class army* unit);
    i32 FindCombatPath(i32 sourceHex, i32 targetHex, class army* unit, ArmyPathTarget attackPath, i32 slowTargetMoat);
    void PushCombatPoint(i32 hex, CombatHexDirection direction, i32 distance, i32 speed);
    searchCell& GetCell(i32 x, i32 y) {
        return (m_storage.cells + x)[MAP_WIDTH * y];
    }
};
extern u8 bIsMoatSlowed[COMBAT_HEX_COUNT];

#endif
