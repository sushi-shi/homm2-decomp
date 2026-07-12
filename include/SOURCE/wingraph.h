#ifndef HOMM2_WINGRAPH_H
#define HOMM2_WINGRAPH_H
#include <va.h>
// Declarations of the free functions DEFINED in wingraph.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

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
DATA(0x004f16ac) extern long (__stdcall *lpDirectDrawCreate)(struct _GUID *, struct IDirectDraw **, struct IUnknown *);
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
