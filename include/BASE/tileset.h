#ifndef HOMM2_BASE_TILESET_H
#define HOMM2_BASE_TILESET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 3 methods, 0 own-virtual, 0 static data.
#include <va.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class tileset {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    short  field_0x4;  // +0x04
    short  field_0x6;  // +0x06
    int    field_0x8;  // +0x08
    int    field_0xc;  // +0x0c
    short  field_0x10;  // +0x10
    short  field_0x12;  // +0x12
    short  field_0x14;  // +0x14
    int    field_0x16;  // +0x16
    // --- constructors ---
    tileset(unsigned long int);
    virtual ~tileset();
};
#pragma pack(pop)
SIZE(tileset, 0x1a);
#endif // HOMM2_BASE_TILESET_H
