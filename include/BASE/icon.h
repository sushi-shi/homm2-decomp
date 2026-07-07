#ifndef HOMM2_BASE_ICON_H
#define HOMM2_BASE_ICON_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 8 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <BASE/resource.h>
// forward declarations:
struct SLimitData;

#pragma pack(push, 1)  // recovered layout is byte-packed
// icon is an asset subclass of resource (slot-0 dtor override).
class icon : public resource {
public:
    // --- own members (base resource occupies 0x00..0x10; widths from access-analysis) ---
    short  field_0x10;  // +0x10
    char  *field_0x12;  // +0x12  (glyph/sprite data; indexed as a byte pointer)
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
