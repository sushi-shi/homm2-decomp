#ifndef HOMM2_BASE_MOUSEMANAGER_H
#define HOMM2_BASE_MOUSEMANAGER_H

#include <va.h>
#include <windows.h>
#include "baseManager.h"

struct tag_message;
class bitmap;
class resource;
class icon;

H2_ENUM_CLASS_BEGIN(MouseCursorType)
    MOUSE_INVALID_CURSOR_TYPE = -1,
    MOUSE_AUTO_CURSOR_TYPE    = -999,
    MOUSE_CURSOR_ADVENTURE    = 0,
    MOUSE_CURSOR_COMBAT       = 1,
    MOUSE_CURSOR_SPELL        = 2
H2_ENUM_CLASS_END(MouseCursorType)

#pragma pack(push, 1)
class mouseManager : public baseManager {
public:
    bitmap* m_savedUnderlying;
    i32 m_cursorFrame;
    icon* m_cursorIcon;
    MouseCursorType m_cursorType;
    i32 m_cursorSizeIndex;
    i32 m_drawnCursorSizeIndex;
    char _pad_0x4e[0x8];
    i32 m_mouseX;
    i32 m_mouseY;
    i32 m_savedLeft;
    i32 m_savedTop;
    i32 m_cursorLeft;
    i32 m_cursorTop;
    i32 m_cursorRight;
    i32 m_cursorBottom;
    i32 m_savedWidth;
    i32 m_savedHeight;
    i32 m_forcePointerUpdate;
    i32 m_cursorReady;
    i32 m_hideCount;
    mouseManager(void);
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void SetPointer(char*, i32, MouseCursorType);
    void SetPointer(i32);
    void NewUpdate(i32);
    void MouseCoords(i32&, i32&);
    void SaveAndDraw(void);
    void RestoreUnderlying(void);
    void ReallyHidePointer(void);
    void ReallyShowPointer(void);
    void HideColorPointer(void);
    void ShowColorPointer(void);
    i32 IsVis(void);
    void CheckUpdateMousePos(void);
    void SetColorMice(i32);
};
#pragma pack(pop)
SIZE(mouseManager, 0x8a);
H2_ENUM_BEGIN(MouseManagerConstant)
    MOUSE_CURSOR_COUNT                 = 96,
    MOUSE_SCREEN_WIDTH                 = 640,
    MOUSE_SCREEN_HEIGHT                = 480,
    MOUSE_SCREEN_CENTER_X              = 320,
    MOUSE_SCREEN_CENTER_Y              = 240,
    MOUSE_SAVED_BITMAP_WIDTH           = 0x42,
    MOUSE_SAVED_BITMAP_HEIGHT          = 0x40,
    MOUSE_CURSOR_BITMAP_WIDTH          = 32,
    MOUSE_CURSOR_MASK_HEIGHT           = 64,
    MOUSE_CURSOR_MASK_ROW_BYTES        = 4,
    MOUSE_CURSOR_COLOR_BYTES           = 0x400,
    MOUSE_CURSOR_AND_BYTES             = 0x100,
    MOUSE_CURSOR_MASK_PLANE_BYTES      = 0x80,
    MOUSE_CURSOR_BITMAP_HEADER_BYTES   = 6,
    MOUSE_CURSOR_BITMAP_PLANES         = 1,
    MOUSE_CURSOR_BITMAP_BITS_PER_PIXEL = 1,
    MOUSE_CURSOR_MASK_HIGH_BIT         = 7,
    MOUSE_SPELL_CURSOR_HOTSPOT         = 15,
    MOUSE_MANAGER_MESSAGE_MASK         = 0x40,
    MOUSE_INVALID_CURSOR_FRAME         = -1,
    MOUSE_RELOAD_CURSOR_FRAME          = -99,
    MOUSE_KEEP_CURRENT_FRAME           = 1000
H2_ENUM_END(MouseManagerConstant)
extern i32 iMouseOffset[4];
extern i8 iMouseSize[MOUSE_CURSOR_COUNT][2];
extern i8 iHotSpot[MOUSE_CURSOR_COUNT][2];
extern b32 gbInSetPointer;
extern i32 bInNewMouseUpdate;
extern BITMAP bmpAndMask[MOUSE_CURSOR_COUNT];
extern HICON hMouseCursor[MOUSE_CURSOR_COUNT];
extern void* cAndBits[MOUSE_CURSOR_COUNT];
extern void* cColorBits[MOUSE_CURSOR_COUNT];
extern ICONINFO IconInfo[MOUSE_CURSOR_COUNT];
extern HBITMAP hbmpAndMask[MOUSE_CURSOR_COUNT];

#endif
