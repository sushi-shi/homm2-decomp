#ifndef HOMM2_BASE_BITMAP_H
#define HOMM2_BASE_BITMAP_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 13 methods, 0 own-virtual, 0 static data.
#include <va.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class bitmap {
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
    bitmap(void);
    bitmap(short int, short int, short int);
    bitmap(unsigned long int);
    virtual ~bitmap();
    // --- methods ---
    void DrawToBufferCareful(short int, short int);
    void DrawToBuffer(short int, short int);
    void DrawToScreen(short int, short int);
    void GrabScreen(short int, short int);
    void GrabBitmap(class bitmap *, short int, short int);
    void GrabBitmapCareful(class bitmap *, short int, short int);
    void CopyTo(class bitmap *, int, int, int, int, int, int);
    void CopyToCareful(class bitmap *, int, int, int, int, int, int);
};
#pragma pack(pop)
SIZE(bitmap, 0x1a);
#endif // HOMM2_BASE_BITMAP_H
