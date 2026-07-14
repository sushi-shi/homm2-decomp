#ifndef HOMM2_WINGRAPH_H
#define HOMM2_WINGRAPH_H
#include <windows.h>
#include <ddraw.h>
#include <va.h>
// Declarations of the free functions DEFINED in wingraph.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

typedef enum WingraphConstant {
    WINGRAPH_WIDTH = 640,
    WINGRAPH_HEIGHT = 480,
    WINGRAPH_COLOR_DEPTH = 8,
    WINGRAPH_PALETTE_SIZE = 256,
    WINGRAPH_PALETTE_VERSION = 0x300,
    WINGRAPH_SYSTEM_PALETTE_SIZE = 10,
    WINGRAPH_SCROLL_MARGIN = 16,
    WINGRAPH_SCROLL_SIZE = WINGRAPH_WIDTH - WINGRAPH_SCROLL_MARGIN * 2,
    WINGRAPH_PAINT_TIMEOUT = 10000,
    WINGRAPH_ERROR_TEXT_SIZE = 200
} WingraphConstant;

#pragma pack(push, 1)
struct _PALETTE {
    WORD version;
    WORD entryCount;
    PALETTEENTRY entries[WINGRAPH_PALETTE_SIZE];
};
#pragma pack(pop)
SIZE(_PALETTE, 0x404);

void DDRestoreDisplayMode(void);
int DDQueryNewPalette(void);
void CreatePrimary(void);
void SetupClipper(void);
void DDInitGraphics(void);
int DDAppPaint(void *, void *);
void DDInitializePalette(void);
int DDSetPalette(void);
struct IDirectDrawSurface * DDCreateSurface(unsigned long int, unsigned long int, int);
void DDSD(int, char *, int);
void DDUpdatePalette(signed char *);
void DDCleanUpWinGraphics(void);
void DDSetFullScreenStatus(int);
int WGQueryNewPalette(void);
void WGInitGraphics(void);
void WGUpdatePalette(signed char *);
void WGInitializePalette(void);
int WGAppPaint(void *, void *);
void WGCleanUpWinGraphics(void);
void ConnectToDLLs(void);
void DisconnectDLLs(void);
void RestoreDisplayMode(void);
int SetPalette(void);
void GetGraphicsInfo(void);
void InitGraphics(void);
int AppPaint(void *, void *);
void InitializePalette(void);
void UpdatePalette(signed char *);
void CleanUpWinGraphics(void);
void SetFullScreenStatus(int);
int QueryNewPalette(void);
int SetGraphicsType(int);


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
extern int giGraphicsType;

// ---- globals (declarations, RVA order) ----
extern int gbWinGAttached;
extern int gbDDrawAttached;
extern long Orientation;
extern void *lpInitWin;
extern int bPaletteInitialized;
extern int giTtlBlts;
extern int gbWinGraphBusy;
extern long (__stdcall *lpDirectDrawCreate)(struct _GUID *, struct IDirectDraw **, struct IUnknown *);
extern struct _PALETTE LogicalPalette;
extern struct IDirectDraw *lpDD;
extern struct IDirectDrawSurface *lpDDSPrimary;
extern struct IDirectDrawSurface *lpDDSOne;
extern struct IDirectDrawClipper *lpClipper;
extern struct IDirectDrawPalette *lpDDPal;
extern int iBusyRetry;
extern int bInDDSD;
extern void *hdcImage;
extern void *gbmOldMonoBitmap;
extern void *hpalApp;
extern void *hWinGLibrary;
extern void *hDDrawLibrary;
extern long lDelayRefresh;
extern long lPaintStart;

#endif // HOMM2_WINGRAPH_H
