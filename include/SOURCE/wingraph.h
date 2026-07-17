#ifndef HOMM2_WINGRAPH_H
#define HOMM2_WINGRAPH_H
#include <windows.h>
#include <ddraw.h>
#include <wing.h>
#include <va.h>
// Declarations of the free functions DEFINED in wingraph.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

H2_ENUM_BEGIN(WingraphConstant)
    WINGRAPH_WIDTH = 640,
    WINGRAPH_HEIGHT = 480,
    WINGRAPH_COLOR_DEPTH = 8,
    WINGRAPH_PALETTE_SIZE = 256,
    WINGRAPH_PALETTE_VERSION = 0x300,
    WINGRAPH_SYSTEM_PALETTE_SIZE = 10,
    WINGRAPH_SCROLL_MARGIN = 16,
    WINGRAPH_SCROLL_SIZE = WINGRAPH_HEIGHT - WINGRAPH_SCROLL_MARGIN * 2,
    WINGRAPH_LIMITED_COMBAT_HEIGHT = 458,
    WINGRAPH_PAINT_ALIGN_MASK = 0xfffc,
    WINGRAPH_LOAD_LIBRARY_SUCCESS = 32,
    WINGRAPH_PAINT_TIMEOUT = 10000,
    WINGRAPH_ERROR_TEXT_SIZE = 200
H2_ENUM_END(WingraphConstant)

H2_ENUM_CLASS_BEGIN(WingraphGraphicsType)
    WINGRAPH_GRAPHICS_WING = 1,
    WINGRAPH_GRAPHICS_DIRECT_DRAW = 2
H2_ENUM_CLASS_END(WingraphGraphicsType)

#pragma pack(push, 1)
struct _PALETTE {
    WORD version;
    WORD entryCount;
    PALETTEENTRY entries[WINGRAPH_PALETTE_SIZE];
};
#pragma pack(pop)
SIZE(_PALETTE, 0x404);

struct _IMAGE {
    BITMAPINFOHEADER header;
    RGBQUAD colors[WINGRAPH_PALETTE_SIZE];
    void* bits;
};
SIZE(_IMAGE, 0x42c);

typedef HRESULT(__stdcall* DirectDrawCreateProc)(
    GUID* guid,
    IDirectDraw** directDraw,
    IUnknown* outerUnknown
);

void DDRestoreDisplayMode(void);
i32 DDQueryNewPalette(void);
void CreatePrimary(void);
void SetupClipper(void);
void DDInitGraphics(void);
i32 DDAppPaint(void*, void*);
void DDInitializePalette(void);
i32 DDSetPalette(void);
struct IDirectDrawSurface* DDCreateSurface(u32l, u32l, i32);
void DDSD(i32, char*, i32);
void DDUpdatePalette(i8*);
void DDCleanUpWinGraphics(void);
void DDSetFullScreenStatus(i32);
i32 WGQueryNewPalette(void);
void WGInitGraphics(void);
void WGUpdatePalette(i8*);
void WGInitializePalette(void);
i32 WGAppPaint(void*, void*);
void WGCleanUpWinGraphics(void);
void ConnectToDLLs(void);
void DisconnectDLLs(void);
void RestoreDisplayMode(void);
i32 SetPalette(void);
void GetGraphicsInfo(void);
void InitGraphics(void);
i32 AppPaint(void*, void*);
void InitializePalette(void);
void UpdatePalette(i8*);
void CleanUpWinGraphics(void);
void SetFullScreenStatus(i32);
i32 QueryNewPalette(void);
i32 SetGraphicsType(i32);

// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
extern WingraphGraphicsType giGraphicsType;

// ---- globals (declarations, RVA order) ----
extern b32 gbWinGAttached;
extern b32 gbDDrawAttached;
extern i32l Orientation;
extern void* lpInitWin;
extern i32 bPaletteInitialized;
extern i32 giTtlBlts;
extern b32 gbWinGraphBusy;
extern DirectDrawCreateProc lpDirectDrawCreate;
extern struct _PALETTE LogicalPalette;
extern struct _IMAGE screenImage;
extern struct IDirectDraw* lpDD;
extern struct IDirectDrawSurface* lpDDSPrimary;
extern struct IDirectDrawSurface* lpDDSOne;
extern struct IDirectDrawClipper* lpClipper;
extern struct IDirectDrawPalette* lpDDPal;
extern i32 iBusyRetry;
extern i32 bInDDSD;
extern HDC hdcImage;
extern HBITMAP gbmOldMonoBitmap;
extern HPALETTE hpalApp;
extern HINSTANCE hWinGLibrary;
extern HINSTANCE hDDrawLibrary;
extern i32l lDelayRefresh;
extern i32l lPaintStart;

#endif // HOMM2_WINGRAPH_H
