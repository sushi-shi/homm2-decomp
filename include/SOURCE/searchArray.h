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
    SEARCH_DIRECTION_OBJECT_MASK      = 0x38,
    SEARCH_OBJECT_TYPE_MASK           = 0xfc,
    SEARCH_BLOCKING_OBJECT_TYPE       = 0xbc
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
    searchNode& GetNode(i32 x, i32 y) {
        return *(m_storage.nodes + y * MAP_WIDTH + x);
    }
    searchArray(void);
    ~searchArray();
    i32 BuildPath(i32, i32, i32, i32, i32);
    void SeedPosition(
        i32,
        i32,
        MapDirection,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32
    );
    void Init(void);
    void Close(void);
    void Clear(void);
    i32 QuickDistance(i32, i32, i32, i32);
    void PushPoint(
        i32,
        i32,
        MapDirection,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32
    );
    void TestPossibleDirections(
        i32,
        i32,
        i8 * const,
        i8* const,
        i32,
        i32
    );
    void SeedCombatPosition(class army*);
    i32 FindCombatPath(i32, i32, class army*, ArmyPathTarget, i32);
    void PushCombatPoint(i32, CombatHexDirection, i32, i32);
    searchCell& GetCell(i32 x, i32 y) {
        return (m_storage.cells + y * MAP_WIDTH)[x];
    }
};
#pragma pack(pop)
extern u8 bIsMoatSlowed[SEARCH_COMBAT_HEX_COUNT];

#endif
