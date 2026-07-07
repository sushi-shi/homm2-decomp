#ifndef HOMM2_BASE_FONT_H
#define HOMM2_BASE_FONT_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 9 methods, 0 own-virtual, 0 static data.
#include <va.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class font {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    short  field_0x4;  // +0x04
    short  field_0x6;  // +0x06
    int    field_0x8;  // +0x08
    int    field_0xc;  // +0x0c
    int    field_0x10;  // +0x10
    int    field_0x14;  // +0x14
    int    field_0x18;  // +0x18
    int    field_0x1c;  // +0x1c
    // --- constructors ---
    font(unsigned long int);
    virtual ~font();
    // --- methods ---
protected:
    void DrawStringExecute(char *, int, int, int, int, int, int, int);   // ?...@font@@IAE... (protected)
public:
    void DrawString(char *, int, int, int);
    int GetCharacterWidth(unsigned char);
    void DrawBoundedString(char *, int, int, int, int, int, int);
    int LineLength(char *, int);
    int LineWidth(char *);
};
#pragma pack(pop)
SIZE(font, 0x20);
#endif // HOMM2_BASE_FONT_H
