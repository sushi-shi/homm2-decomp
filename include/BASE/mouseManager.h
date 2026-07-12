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
    bitmap *m_savedUnderlying;  // +0x36  saved-underlying bitmap
    int    m_cursorFrame;  // +0x3a
    icon  *m_cursorIcon;  // +0x3e  the loaded cursor icon
    int    m_cursorType;  // +0x42
    int    m_cursorSizeIndex;  // +0x46
    int    field_0x4a;  // +0x4a
    char _pad_0x4e[0x8];
    int    m_mouseX;  // +0x56
    int    m_mouseY;  // +0x5a
    int    m_savedW;  // +0x5e
    int    m_savedH;  // +0x62
    int    field_0x66;  // +0x66
    int    field_0x6a;  // +0x6a
    int    field_0x6e;  // +0x6e
    int    field_0x72;  // +0x72
    int    field_0x76;  // +0x76
    int    field_0x7a;  // +0x7a
    int    field_0x7e;  // +0x7e
    int    m_cursorReady;  // +0x82
    int    m_hideCount;  // +0x86
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
extern int *iMouseOffset;
extern signed char iMouseSize[96][2];
extern signed char iHotSpot[96][2];
extern int gbInSetPointer;
extern int bInNewMouseUpdate;
extern struct tagBITMAP *bmpAndMask;
extern void *hMouseCursor[96];
extern void *cAndBits[96];
extern void *cColorBits[98];
extern struct _ICONINFO *IconInfo;
extern void *hbmpAndMask[106];

#endif // HOMM2_BASE_MOUSEMANAGER_H
