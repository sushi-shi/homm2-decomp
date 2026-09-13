#ifndef HOMM2_SOURCE_SEARCHARRAY_H
#define HOMM2_SOURCE_SEARCHARRAY_H

#include <Ints.h>
#include <SOURCE/combatTypes.h>

class army;
extern i32 MAP_WIDTH;

typedef enum SearchConstant {
    SEARCH_DIRECTION_COUNT            = 8,
    SEARCH_QUEUE_CAPACITY             = 1024,
    SEARCH_PATH_CAPACITY              = 256,
    SEARCH_FLAG_BIT_COUNT             = 1,
    SEARCH_DIRECTION_BIT_COUNT        = 4,
    SEARCH_CELL_PAD_SIZE              = 4,
    SEARCH_PATH_HEADER_SIZE           = 3,
    SEARCH_AI_PATH_HEADER_SIZE        = 4,
    SEARCH_COMBAT_HEX_COUNT           = 117,
    SEARCH_MAX_COST                   = 999999,
    SEARCH_TARGET_COST_WINDOW         = 75,
    SEARCH_MONSTER_RESEED_WINDOW      = 300,
    SEARCH_DIAGONAL_COST_MASK         = 1,
    SEARCH_CELL_UNREACHABLE           = 0x08,
    SEARCH_MAP_BLOCKED                = 0x80,
    SEARCH_CELL_BLOCKED               = 0x80,
    SEARCH_CLEAR_GROUND_TILESET       = 0x2f,
    SEARCH_INVALID_COORDINATE         = -1,
    SEARCH_NO_OBJECT                  = 0xFF,
    SEARCH_DIRECTION_EDGE_OBJECT_MASK = 0x83,
    SEARCH_DIRECTION_OBJECT_MASK      = 0x38
} SearchConstant;

#pragma pack(push, 1)
struct searchCell {
    u16 cost;
    u16 previous;
    u8 flags : SEARCH_FLAG_BIT_COUNT;
    char pad[SEARCH_CELL_PAD_SIZE];
};
#pragma pack(pop)

union searchStorage {
    struct searchCell* cells;
    struct searchNode* nodes;
    struct {
        char pad[SEARCH_PATH_HEADER_SIZE];
        u8 directions[SEARCH_PATH_CAPACITY + 1];
    } path;
    struct {
        char pad[SEARCH_AI_PATH_HEADER_SIZE];
        u8 directions[SEARCH_PATH_CAPACITY];
    } aiPath;
};

#pragma pack(push, 1)
struct searchNode {
    u8 x;
    u8 y;
    u16 distance;
    u8 visited : SEARCH_FLAG_BIT_COUNT;
    u8 unknownFlag : SEARCH_FLAG_BIT_COUNT;
    u8 rvFlag1 : SEARCH_FLAG_BIT_COUNT;
    u8 rvFlag2 : SEARCH_FLAG_BIT_COUNT;
    u8 direction : SEARCH_DIRECTION_BIT_COUNT;
    union {
        struct {
            u8 adjacentMonsterX;
            u8 adjacentMonsterY;
            u8 previousFlags;
            u8 terrain;
        };
        struct {
            i8 valueX;
            i8 valueY;
            i8 previousX;
            i8 previousY;
        };
    };
};
#pragma pack(pop)

#pragma pack(push, 1)
class searchArray {
public:
    union {
        struct {
            i32 m_queueSize;
            i32 m_queueCursor;
        };
        struct {
            u32 m_queueCount;
            u32 m_maxQueueCount;
        };
    };
    i32 m_pathLength;
    union {
        struct {
            i32 m_lastY;
            i32 m_lastX;
        };
        struct {
            i32 m_specialTargetX;
            i32 m_specialTargetY;
        };
    };
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
        i32 scanMap
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
        i32 mobility,
        i32 unknownFlag,
        i32 rvFlag1,
        i32 valueX,
        i32 valueY,
        i32 rvFlag2,
        i32 previousX,
        i32 previousY
    );
    void TestPossibleDirections(
        i32 x,
        i32 y,
        i8 * const terrain,
        i8* const occupied,
        i32 allowOccupied,
        i32 waterMode
    );
    void SeedCombatPosition(class army* unit);
    i32 FindCombatPath(i32 sourceHex, i32 targetHex, class army* unit, ArmyPathTarget attackPath, i32 ignoreTargetMoat);
    void PushCombatPoint(i32 hex, CombatHexDirection direction, i32 distance, i32 speed);
    searchCell& GetCell(i32 x, i32 y) {
        return (m_storage.cells + x)[MAP_WIDTH * y];
    }
};
#pragma pack(pop)
extern u8 bIsMoatSlowed[SEARCH_COMBAT_HEX_COUNT];

#endif
