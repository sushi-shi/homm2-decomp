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
DATA(0x004f1290) extern int giGraphicsType;

// ---- globals (declarations, RVA order) ----
DATA(0x004f1288) extern int gbWinGAttached;
DATA(0x004f128c) extern int gbDDrawAttached;
DATA(0x004f1294) extern long Orientation;
DATA(0x004f169c) extern void *lpInitWin;
DATA(0x004f16a0) extern int bPaletteInitialized;
DATA(0x004f16a4) extern int giTtlBlts;
DATA(0x004f16a8) extern int gbWinGraphBusy;
DATA(0x004f16ac) extern long (__stdcall *lpDirectDrawCreate)(struct _GUID *, struct IDirectDraw **, struct IUnknown *);
DATA(0x004f16b0) extern struct IDirectDraw *lpDD;
DATA(0x004f16b4) extern struct IDirectDrawSurface *lpDDSPrimary;
DATA(0x004f16b8) extern struct IDirectDrawSurface *lpDDSOne;
DATA(0x004f16bc) extern struct IDirectDrawClipper *lpClipper;
DATA(0x004f16c0) extern struct IDirectDrawPalette *lpDDPal;
DATA(0x004f17b0) extern int iBusyRetry;
DATA(0x004f1a14) extern int bInDDSD;
DATA(0x004f1e78) extern void *hdcImage;
DATA(0x004f1e7c) extern void *gbmOldMonoBitmap;
DATA(0x004f1e80) extern void *hpalApp;
DATA(0x004f1f60) extern void *hWinGLibrary;
DATA(0x004f1f64) extern void *hDDrawLibrary;
DATA(0x004f2080) extern long lDelayRefresh;
DATA(0x00525168) extern long lPaintStart;

#endif // HOMM2_WINGRAPH_H
