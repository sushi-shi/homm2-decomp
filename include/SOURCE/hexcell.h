#ifndef HOMM2_SOURCE_HEXCELL_H
#define HOMM2_SOURCE_HEXCELL_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 8 methods, 0 own-virtual, 0 static data.
#include <va.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class hexcell {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    short  field_0x0;  // +0x00
    short  field_0x2;  // +0x02
    char _pad_0x4[0xa];
    char   field_0xe;  // +0x0e
    char   field_0xf;  // +0x0f
    char   field_0x10;  // +0x10
    char   field_0x11;  // +0x11
    char   field_0x12;  // +0x12
    int    field_0x13;  // +0x13
    char _pad_0x17[0x4b];  // tail pad to sizeof
    // --- constructors ---
    hexcell(void);
    // --- methods ---
    void DrawGround(void);
    void DrawLowerDeadOccupants(void);
    void DrawUpperDeadOccupant(void);
    void DrawOccupant(int, int);
    void DrawTower(int);
    void DrawClouds(void);
    void DrawObstacle(void);
};
#pragma pack(pop)
SIZE(hexcell, 0x62);
#endif // HOMM2_SOURCE_HEXCELL_H
