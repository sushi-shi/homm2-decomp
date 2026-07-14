#ifndef HOMM2_SOURCE_SEARCHARRAY_H
#define HOMM2_SOURCE_SEARCHARRAY_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 13 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class army;
extern int MAP_WIDTH;

typedef enum SearchConstant {
    SEARCH_DIRECTION_COUNT = 8,
    SEARCH_DIRECTION_REVERSE = 4,
    SEARCH_DIRECTION_MASK = 7,
    SEARCH_QUEUE_CAPACITY = 1024,
    SEARCH_PATH_CAPACITY = 256,
    SEARCH_MAX_COST = 999999,
    SEARCH_TARGET_COST_WINDOW = 75,
    SEARCH_MONSTER_RESEED_WINDOW = 300,
    SEARCH_MAP_BLOCKED = 0x80,
    SEARCH_CELL_BLOCKED = 0x80,
    SEARCH_TRIGGER_MASK = 0x7f,
    SEARCH_TRIGGER_MONSTER = 0x18,
    SEARCH_TRIGGER_BOAT = 0x1c,
    SEARCH_TRIGGER_IGNORE_FIRST = 0x24,
    SEARCH_TRIGGER_IGNORE_SECOND = 0x27,
    SEARCH_TRIGGER_TOWN = 0x2a,
    SEARCH_TRIGGER_TOWN_ALT = 0x2b,
    SEARCH_WATER_ENTRY_FIRST = 0xaa,
    SEARCH_WATER_ENTRY_SECOND = 0xab,
    SEARCH_WATER_ENTRY_THIRD = 0xa0,
    SEARCH_CLEAR_GROUND_TILESET = 0x2f,
    SEARCH_INVALID_COORDINATE = -1,
    SEARCH_NO_OBJECT = 0xff,
    SEARCH_DIRECTION_OBJECT_MASK = 0x38,
    SEARCH_OBJECT_TYPE_MASK = 0xfc,
    SEARCH_BLOCKING_OBJECT_TYPE = 0xbc
} SearchConstant;

#pragma pack(push, 1)
struct searchCell {
    unsigned short cost;
    unsigned short previous;
    unsigned char flags : 1;
    char pad[4];
};
#pragma pack(pop)
SIZE(searchCell, 9);

union searchStorage {
    struct searchCell *cells;
    struct searchNode *nodes;
    struct {
        char pad[3];
        signed char directions[SEARCH_PATH_CAPACITY + 1];
    } path;
    struct {
        char pad[4];
        signed char directions[0x100];
    } aiPath;
};

#pragma pack(push, 1)
struct searchNode {
    unsigned char x;
    unsigned char y;
    unsigned short distance;
    unsigned char visited : 1;
    unsigned char unknownFlag : 1;
    unsigned char rvFlag1 : 1;
    unsigned char rvFlag2 : 1;
    unsigned char direction : 4;
    unsigned char adjacentMonsterX;
    unsigned char adjacentMonsterY;
    unsigned char previousFlags;
    unsigned char terrain;
};
#pragma pack(pop)
SIZE(searchNode, 9);

#pragma pack(push, 1)  // recovered layout is byte-packed
class searchArray {
public:
    int    m_queueSize;  // +0x00
    int    m_queueCursor;  // +0x04
    int    m_pathLength;  // +0x08
    int    m_lastY;  // +0x0c
    int    m_lastX;  // +0x10
    searchNode m_queue[SEARCH_QUEUE_CAPACITY];  // +0x14
    searchStorage m_storage;  // +0x2414, path directions overlap the search-cell pointer
    searchNode *GetRow(int y, int width) { return m_storage.nodes + y * (width | 0); }
    searchNode &GetNode(int x, int y) {
        return *(m_storage.nodes + y * (MAP_WIDTH | 0) + x);
    }
    // --- constructors ---
    searchArray(void);
    ~searchArray();
    // --- methods ---
    int BuildPath(int, int, int, int, int);
    void SeedPosition(int, int, int, int, int, int, int, int, int, int, int, int);
    void Init(void);
    void Close(void);
    void Clear(void);
    int QuickDistance(int, int, int, int);
    void PushPoint(int, int, int, int, int, int, int, int, int, int, int, int);
    void TestPossibleDirections(int, int, signed char * const, signed char * const, int, int);
    void SeedCombatPosition(class army *);
    int FindCombatPath(int, int, class army *, int, int);
    void PushCombatPoint(int, int, int, int);
    searchCell &GetCell(int x, int y) { return (m_storage.cells + y * MAP_WIDTH)[x]; }
};
#pragma pack(pop)
SIZE(searchArray, 0x2518);
// ---- globals (declarations, RVA order) ----
extern unsigned char *bIsMoatSlowed;

#endif // HOMM2_SOURCE_SEARCHARRAY_H
