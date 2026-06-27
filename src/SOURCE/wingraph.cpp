// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\wingraph.obj   from: (directly linked into exe)
// functions: 32   data: 25
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <rva.h>
RVA(0x00435290, 0x55)
// void DDRestoreDisplayMode(void);

RVA(0x004352e5, 0x46)
// int DDQueryNewPalette(void);

RVA(0x0043532b, 0x94)
// void CreatePrimary(void);

RVA(0x004353bf, 0xe3)
// void SetupClipper(void);

RVA(0x004354a2, 0x15f)
// void DDInitGraphics(void);

RVA(0x00435601, 0x530)
// int DDAppPaint(void *, void *);

RVA(0x00435b31, 0x13c)
// void DDInitializePalette(void);

RVA(0x00435c6d, 0xaf)
// int DDSetPalette(void);

RVA(0x00435d1c, 0x133)
// struct IDirectDrawSurface * DDCreateSurface(unsigned long int, unsigned long int, int);

RVA(0x00435e4f, 0x5d2)
// void DDSD(int, char *, int);

RVA(0x00436421, 0x118)
// void DDUpdatePalette(signed char *);

RVA(0x00436539, 0x177)
// void DDCleanUpWinGraphics(void);

RVA(0x004366b0, 0x305)
// void DDSetFullScreenStatus(int);

RVA(0x004369b5, 0x89)
// int WGQueryNewPalette(void);

RVA(0x00436a3e, 0x151)
// void WGInitGraphics(void);

RVA(0x00436b8f, 0x1e7)
// void WGUpdatePalette(signed char *);

RVA(0x00436d76, 0x1d5)
// void WGInitializePalette(void);

RVA(0x00436f4b, 0x1c4)
// int WGAppPaint(void *, void *);

RVA(0x0043710f, 0x78)
// void WGCleanUpWinGraphics(void);

RVA(0x00437187, 0x80)
// void ConnectToDLLs(void);

RVA(0x00437207, 0x29)
// void DisconnectDLLs(void);

RVA(0x00437230, 0x2c)
// void RestoreDisplayMode(void);

RVA(0x0043725c, 0x2e)
// int SetPalette(void);

RVA(0x0043728a, 0x7e)
// void GetGraphicsInfo(void);

RVA(0x00437308, 0xa0)
// void InitGraphics(void);

RVA(0x004373a8, 0x46)
// int AppPaint(void *, void *);

RVA(0x004373ee, 0x2c)
// void InitializePalette(void);

RVA(0x0043741a, 0x38)
// void UpdatePalette(signed char *);

RVA(0x00437452, 0x31)
// void CleanUpWinGraphics(void);

RVA(0x00437483, 0xe1)
// void SetFullScreenStatus(int);

RVA(0x00437564, 0x31)
// int QueryNewPalette(void);

RVA(0x00437595, 0x286)
// int SetGraphicsType(int);

// ---- data / globals / vtables ----
DATA(0x004f1288)  // int gbWinGAttached
DATA(0x004f128c)  // int gbDDrawAttached
DATA(0x004f1290)  // int giGraphicsType
DATA(0x004f1294)  // long int Orientation
DATA(0x004f1298)  // struct _PALETTE LogicalPalette
DATA(0x004f169c)  // void * lpInitWin
DATA(0x004f16a0)  // int bPaletteInitialized
DATA(0x004f16a4)  // int giTtlBlts
DATA(0x004f16a8)  // int gbWinGraphBusy
DATA(0x004f16ac)  // long int (*)(struct _GUID *, struct IDirectDraw * *, struct IUnknown *) lpDirectDrawCreate
DATA(0x004f16b0)  // struct IDirectDraw * lpDD
DATA(0x004f16b4)  // struct IDirectDrawSurface * lpDDSPrimary
DATA(0x004f16b8)  // struct IDirectDrawSurface * lpDDSOne
DATA(0x004f16bc)  // struct IDirectDrawClipper * lpClipper
DATA(0x004f16c0)  // struct IDirectDrawPalette * lpDDPal
DATA(0x004f17b0)  // int iBusyRetry
DATA(0x004f1a14)  // int bInDDSD
DATA(0x004f1e78)  // void * hdcImage
DATA(0x004f1e7c)  // void * gbmOldMonoBitmap
DATA(0x004f1e80)  // void * hpalApp
DATA(0x004f1f60)  // void * hWinGLibrary
DATA(0x004f1f64)  // void * hDDrawLibrary
DATA(0x004f2080)  // long int lDelayRefresh
DATA(0x00525168)  // long int lPaintStart
DATA(0x00525180)  // struct _IMAGE screenImage
