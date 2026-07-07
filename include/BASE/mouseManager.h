#ifndef HOMM2_BASE_MOUSEMANAGER_H
#define HOMM2_BASE_MOUSEMANAGER_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 17 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "baseManager.h"
// forward declarations:
struct tag_message;
class bitmap;
class resource;
class icon;

#pragma pack(push, 1)  // recovered layout is byte-packed
class mouseManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    bitmap *field_0x36;  // +0x36  saved-underlying bitmap
    int    field_0x3a;  // +0x3a
    icon  *field_0x3e;  // +0x3e  the loaded cursor icon
    int    field_0x42;  // +0x42
    int    field_0x46;  // +0x46
    int    field_0x4a;  // +0x4a
    char _pad_0x4e[0x8];
    int    field_0x56;  // +0x56
    int    field_0x5a;  // +0x5a
    int    field_0x5e;  // +0x5e
    int    field_0x62;  // +0x62
    int    field_0x66;  // +0x66
    int    field_0x6a;  // +0x6a
    int    field_0x6e;  // +0x6e
    int    field_0x72;  // +0x72
    int    field_0x76;  // +0x76
    int    field_0x7a;  // +0x7a
    int    field_0x7e;  // +0x7e
    int    field_0x82;  // +0x82
    int    field_0x86;  // +0x86
    // --- constructors ---
    mouseManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void SetPointer(char *, int, int);
    void SetPointer(int);
    void NewUpdate(int);
    void MouseCoords(int &, int &);
    void SaveAndDraw(void);
    void RestoreUnderlying(void);
    void ReallyHidePointer(void);
    void ReallyShowPointer(void);
    void HideColorPointer(void);
    void ShowColorPointer(void);
    int IsVis(void);
    void CheckUpdateMousePos(void);
    void SetColorMice(int);
};
#pragma pack(pop)
SIZE(mouseManager, 0x8a);
// ---- globals (declarations, RVA order) ----
DATA(0x0051ebc8) extern int *iMouseOffset;
DATA(0x0051ebd8) extern signed char iMouseSize[96][2];
DATA(0x0051ec98) extern signed char iHotSpot[96][2];
DATA(0x0051ed58) extern int gbInSetPointer;
DATA(0x0051ed5c) extern int bInNewMouseUpdate;
DATA(0x00533240) extern struct tagBITMAP *bmpAndMask;
DATA(0x00533b58) extern void *hMouseCursor[96];
DATA(0x00533cd8) extern void *cAndBits[96];
DATA(0x00533e58) extern void *cColorBits[98];
DATA(0x00533fe0) extern struct _ICONINFO *IconInfo;
DATA(0x00534760) extern void *hbmpAndMask[106];

#endif // HOMM2_BASE_MOUSEMANAGER_H
