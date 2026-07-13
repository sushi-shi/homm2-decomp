#ifndef HOMM2_SOURCE_SEARCHARRAY_H
#define HOMM2_SOURCE_SEARCHARRAY_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 13 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class army;
extern int MAP_WIDTH;

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
        unsigned char directions[0x101];
    } path;
    struct {
        char pad[4];
        signed char directions[0x100];
    } aiPath;
};

#pragma pack(push, 1)
struct searchNode {
    char pad0[2];
    unsigned short distance;
    unsigned char visited : 1;
    unsigned char flags : 7;
    char pad1[4];
};
#pragma pack(pop)
SIZE(searchNode, 9);

#pragma pack(push, 1)  // recovered layout is byte-packed
class searchArray {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    int    field_0x0;  // +0x00
    int    field_0x4;  // +0x04
    int    m_pathLength;  // +0x08
    int    field_0xc;  // +0x0c
    int    field_0x10;  // +0x10
    int    field_0x14;  // +0x14
    char _pad_0x18[0x23fc];
    searchStorage m_storage;  // +0x2414, path directions overlap the search-cell pointer
    searchNode *GetRow(int y, int width) { return m_storage.nodes + y * (width | 0); }
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
