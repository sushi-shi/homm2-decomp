#ifndef HOMM2_BASE_ICON_H
#define HOMM2_BASE_ICON_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 8 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
struct SLimitData;

#pragma pack(push, 1)  // recovered layout is byte-packed
class icon {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    short  field_0x4;  // +0x04
    short  field_0x6;  // +0x06
    int    field_0x8;  // +0x08
    int    field_0xc;  // +0x0c
    short  field_0x10;  // +0x10
    int    field_0x12;  // +0x12
    // --- constructors ---
    icon(unsigned long int);
    virtual ~icon();
    // --- methods ---
    void DrawToBuffer(int, int, int, int);
    int CombatClipDrawToBuffer(int, int, int, struct SLimitData *, int, int, unsigned char *, signed char *);
    void ClipFillToBuffer(int, int, int, int, int, int, int, int, int);
    void FillToBuffer(int, int, int, int, int, struct SLimitData *);
    void DimToBuffer(int, int, int, int);
};
#pragma pack(pop)
SIZE(icon, 0x16);
#endif // HOMM2_BASE_ICON_H
