// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\wingraph.obj   from: (directly linked into exe)
// functions: 32   data: 25
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/wingraph.h>
VA(0x00435290, 0x55)
void DDRestoreDisplayMode(void) {}

VA(0x004352e5, 0x46)
int DDQueryNewPalette(void) { return 0; }

VA(0x0043532b, 0x94)
void CreatePrimary(void) {}

VA(0x004353bf, 0xe3)
void SetupClipper(void) {}

VA(0x004354a2, 0x15f)
void DDInitGraphics(void) {}

VA(0x00435601, 0x530)
int DDAppPaint(void *, void *) { return 0; }

VA(0x00435b31, 0x13c)
void DDInitializePalette(void) {}

VA(0x00435c6d, 0xaf)
int DDSetPalette(void) { return 0; }

VA(0x00435d1c, 0x133)
struct IDirectDrawSurface * DDCreateSurface(unsigned long int, unsigned long int, int) { return 0; }

VA(0x00435e4f, 0x5d2)
void DDSD(int, char *, int) {}

VA(0x00436421, 0x118)
void DDUpdatePalette(signed char *) {}

VA(0x00436539, 0x177)
void DDCleanUpWinGraphics(void) {}

VA(0x004366b0, 0x305)
void DDSetFullScreenStatus(int) {}

VA(0x004369b5, 0x89)
int WGQueryNewPalette(void) { return 0; }

VA(0x00436a3e, 0x151)
void WGInitGraphics(void) {}

VA(0x00436b8f, 0x1e7)
void WGUpdatePalette(signed char *) {}

VA(0x00436d76, 0x1d5)
void WGInitializePalette(void) {}

VA(0x00436f4b, 0x1c4)
int WGAppPaint(void *, void *) { return 0; }

VA(0x0043710f, 0x78)
void WGCleanUpWinGraphics(void) {}

VA(0x00437187, 0x80)
void ConnectToDLLs(void) {}

VA(0x00437207, 0x29)
void DisconnectDLLs(void) {}

VA(0x00437230, 0x2c)
void RestoreDisplayMode(void) {}

VA(0x0043725c, 0x2e)
int SetPalette(void) { return 0; }

VA(0x0043728a, 0x7e)
void GetGraphicsInfo(void) {}

VA(0x00437308, 0xa0)
void InitGraphics(void) {}

VA(0x004373a8, 0x46)
int AppPaint(void *, void *) { return 0; }

VA(0x004373ee, 0x2c)
void InitializePalette(void) {}

VA(0x0043741a, 0x38)
void UpdatePalette(signed char *) {}

VA(0x00437452, 0x31)
void CleanUpWinGraphics(void) {}

VA(0x00437483, 0xe1)
void SetFullScreenStatus(int) {}

VA(0x00437564, 0x31)
int QueryNewPalette(void) { return 0; }

VA(0x00437595, 0x286)
int SetGraphicsType(int) { return 0; }

// ---- globals (definitions, RVA order) ----
int gbWinGAttached;
int gbDDrawAttached;
int giGraphicsType;
long Orientation;
void *lpInitWin;
int bPaletteInitialized;
int giTtlBlts;
int gbWinGraphBusy;
struct IDirectDraw *lpDD;
struct IDirectDrawSurface *lpDDSPrimary;
struct IDirectDrawSurface *lpDDSOne;
struct IDirectDrawClipper *lpClipper;
struct IDirectDrawPalette *lpDDPal;
int iBusyRetry;
int bInDDSD;
void *hdcImage;
void *gbmOldMonoBitmap;
void *hpalApp;
void *hWinGLibrary;
void *hDDrawLibrary;
long lDelayRefresh;
long lPaintStart;
