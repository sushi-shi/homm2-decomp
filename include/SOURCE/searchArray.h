#ifndef HOMM2_SOURCE_SEARCHARRAY_H
#define HOMM2_SOURCE_SEARCHARRAY_H

#include <va.h>

class army;
extern i32 MAP_WIDTH;

H2_ENUM_BEGIN(SearchConstant)
    SEARCH_DIRECTION_COUNT            = 8,
    SEARCH_DIRECTION_REVERSE          = 4,
    SEARCH_DIRECTION_MASK             = 7,
    SEARCH_QUEUE_CAPACITY             = 1024,
    SEARCH_PATH_CAPACITY              = 256,
    SEARCH_MAX_COST                   = 999999,
    SEARCH_TARGET_COST_WINDOW         = 75,
    SEARCH_MONSTER_RESEED_WINDOW      = 300,
    SEARCH_TERRAIN_WATER              = 0,
    SEARCH_TERRAIN_ROAD               = 9,
    SEARCH_DIAGONAL_COST_MASK         = 1,
    SEARCH_CELL_UNREACHABLE           = 0x08,
    SEARCH_MAP_BLOCKED                = 0x80,
    SEARCH_CELL_BLOCKED               = 0x80,
    SEARCH_TRIGGER_PRESENT            = 0x80,
    SEARCH_TRIGGER_MASK               = 0x7f,
    SEARCH_TRIGGER_MONSTER            = 0x18,
    SEARCH_TRIGGER_BOAT               = 0x1c,
    SEARCH_TRIGGER_IGNORE_FIRST       = 0x24,
    SEARCH_TRIGGER_IGNORE_SECOND      = 0x27,
    SEARCH_TRIGGER_TOWN               = 0x2a,
    SEARCH_TRIGGER_TOWN_ALT           = 0x2b,
    SEARCH_WATER_ENTRY_FIRST          = 0xaa,
    SEARCH_WATER_ENTRY_SECOND         = 0xab,
    SEARCH_WATER_ENTRY_THIRD          = 0xa0,
    SEARCH_CLEAR_GROUND_TILESET       = 0x2f,
    SEARCH_INVALID_COORDINATE         = -1,
    SEARCH_NO_OBJECT                  = 0xFF,
    SEARCH_DIRECTION_EDGE_OBJECT_MASK = 0x83,
    SEARCH_DIRECTION_OBJECT_MASK      = 0x38,
    SEARCH_OBJECT_TYPE_MASK           = 0xfc,
    SEARCH_BLOCKING_OBJECT_TYPE       = 0xbc
H2_ENUM_END(SearchConstant)

#pragma pack(push, 1)
struct searchCell {
    u16 cost;
    u16 previous;
    u8 flags : 1;
    char pad[4];
};
#pragma pack(pop)
SIZE(searchCell, 9);

union searchStorage {
    struct searchCell* cells;
    struct searchNode* nodes;
    struct {
        char pad[3];
        i8 directions[SEARCH_PATH_CAPACITY + 1];
    } path;
    struct {
        char pad[4];
        i8 directions[0x100];
    } aiPath;
};

#pragma pack(push, 1)
struct searchNode {
    u8 x;
    u8 y;
    u16 distance;
    u8 visited : 1;
    u8 unknownFlag : 1;
    u8 rvFlag1 : 1;
    u8 rvFlag2 : 1;
    u8 direction : 4;
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
SIZE(searchNode, 9);

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
        return m_storage.nodes + y * (width | 0);
    }
    searchNode& GetNode(i32 x, i32 y) {
        return *(m_storage.nodes + y * (MAP_WIDTH | 0) + x);
    }
    searchArray(void);
    ~searchArray();
    i32 BuildPath(i32, i32, i32, i32, i32);
    void SeedPosition(i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32);
    void Init(void);
    void Close(void);
    void Clear(void);
    i32 QuickDistance(i32, i32, i32, i32);
    void PushPoint(i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32);
    void TestPossibleDirections(i32, i32, i8* const, i8* const, i32, i32);
    void SeedCombatPosition(class army*);
    i32 FindCombatPath(i32, i32, class army*, i32, i32);
    void PushCombatPoint(i32, i32, i32, i32);
    searchCell& GetCell(i32 x, i32 y) {
        return (m_storage.cells + y * MAP_WIDTH)[x];
    }
};
#pragma pack(pop)
SIZE(searchArray, 0x2518);
extern u8 bIsMoatSlowed[117];

#endif
