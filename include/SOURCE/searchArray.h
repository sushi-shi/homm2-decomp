#ifndef HOMM2_SOURCE_SEARCHARRAY_H
#define HOMM2_SOURCE_SEARCHARRAY_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 13 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class army;

#pragma pack(push, 1)  // recovered layout is byte-packed
class searchArray {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    int    field_0x0;  // +0x00
    int    field_0x4;  // +0x04
    int    field_0x8;  // +0x08
    int    field_0xc;  // +0x0c
    int    field_0x10;  // +0x10
    int    field_0x14;  // +0x14
    char _pad_0x18[0x2500];  // tail pad to sizeof
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
};
#pragma pack(pop)
SIZE(searchArray, 0x2518);
#endif // HOMM2_SOURCE_SEARCHARRAY_H
