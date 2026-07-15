#ifndef HOMM2_BASE_MOUSEMANAGER_H
#define HOMM2_BASE_MOUSEMANAGER_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 17 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include <windows.h>
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
    int    m_drawnCursorSizeIndex;  // +0x4a  size-table entry currently drawn/saved
    char _pad_0x4e[0x8];
    int    m_mouseX;  // +0x56
    int    m_mouseY;  // +0x5a
    int    m_savedLeft;  // +0x5e  clipped left edge of saved underlying pixels
    int    m_savedTop;  // +0x62  clipped top edge of saved underlying pixels
    int    m_cursorLeft;  // +0x66  unclipped cursor draw origin
    int    m_cursorTop;  // +0x6a  unclipped cursor draw origin
    int    m_cursorRight;  // +0x6e  clipped inclusive right edge
    int    m_cursorBottom;  // +0x72  clipped inclusive bottom edge
    int    m_savedWidth;  // +0x76  clipped saved-underlying width
    int    m_savedHeight;  // +0x7a  clipped saved-underlying height
    int    m_forcePointerUpdate;  // +0x7e
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
typedef enum MouseManagerConstant {
    MOUSE_CURSOR_COUNT = 96,
    MOUSE_SCREEN_WIDTH = 640,
    MOUSE_SCREEN_HEIGHT = 480,
    MOUSE_SCREEN_CENTER_X = 320,
    MOUSE_SCREEN_CENTER_Y = 240,
    MOUSE_SAVED_BITMAP_WIDTH = 0x42,
    MOUSE_SAVED_BITMAP_HEIGHT = 0x40,
    MOUSE_CURSOR_BITMAP_WIDTH = 32,
    MOUSE_CURSOR_MASK_HEIGHT = 64,
    MOUSE_CURSOR_MASK_ROW_BYTES = 4,
    MOUSE_CURSOR_COLOR_BYTES = 0x400,
    MOUSE_CURSOR_AND_BYTES = 0x100,
    MOUSE_CURSOR_MASK_PLANE_BYTES = 0x80,
    MOUSE_CURSOR_BITMAP_HEADER_BYTES = 6,
    MOUSE_CURSOR_BITMAP_PLANES = 1,
    MOUSE_CURSOR_BITMAP_BITS_PER_PIXEL = 1,
    MOUSE_CURSOR_MASK_HIGH_BIT = 7,
    MOUSE_SPELL_CURSOR_HOTSPOT = 15,
    MOUSE_MANAGER_MESSAGE_MASK = 0x40,
    MOUSE_INVALID_CURSOR_TYPE = -1,
    MOUSE_INVALID_CURSOR_FRAME = -1,
    MOUSE_RELOAD_CURSOR_FRAME = -99,
    MOUSE_KEEP_CURRENT_FRAME = 1000,
    MOUSE_AUTO_CURSOR_TYPE = -999
} MouseManagerConstant;
typedef enum MouseCursorType {
    MOUSE_CURSOR_ADVENTURE = 0,
    MOUSE_CURSOR_COMBAT = 1,
    MOUSE_CURSOR_SPELL = 2
} MouseCursorType;
// ---- globals (declarations, RVA order) ----
extern int iMouseOffset[4];
extern signed char iMouseSize[MOUSE_CURSOR_COUNT][2];
extern signed char iHotSpot[MOUSE_CURSOR_COUNT][2];
extern int gbInSetPointer;
extern int bInNewMouseUpdate;
extern BITMAP bmpAndMask[MOUSE_CURSOR_COUNT];
extern HICON hMouseCursor[MOUSE_CURSOR_COUNT];
extern void *cAndBits[MOUSE_CURSOR_COUNT];
extern void *cColorBits[MOUSE_CURSOR_COUNT];
extern ICONINFO IconInfo[MOUSE_CURSOR_COUNT];
extern HBITMAP hbmpAndMask[MOUSE_CURSOR_COUNT];

#endif // HOMM2_BASE_MOUSEMANAGER_H
