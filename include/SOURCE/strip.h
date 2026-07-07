#ifndef HOMM2_SOURCE_STRIP_H
#define HOMM2_SOURCE_STRIP_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 5 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class armyGroup;

#pragma pack(push, 1)  // recovered layout is byte-packed
class strip {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    int    field_0x0;  // +0x00
    char _pad_0x4[0x18];
    int    field_0x1c;  // +0x1c
    int    field_0x20;  // +0x20
    int    field_0x24;  // +0x24
    int    field_0x28;  // +0x28
    int    field_0x2c;  // +0x2c
    char _pad_0x30[0x10];
    int    field_0x40;  // +0x40
    int    field_0x44;  // +0x44
    char _pad_0x48[0x28];
    int    field_0x70;  // +0x70
    int    field_0x74;  // +0x74
    int    field_0x78;  // +0x78
    int    field_0x7c;  // +0x7c
    int    field_0x80;  // +0x80
    // --- constructors ---
    strip(int, int, int, unsigned long int, int, class armyGroup *, int, int, int);
    ~strip();
    // --- methods ---
    void Draw(void);
    void DrawIcons(int);
    void DrawFrame(void);
};
#pragma pack(pop)
SIZE(strip, 0x84);
#endif // HOMM2_SOURCE_STRIP_H
