#ifndef _INC_WING
#define _INC_WING
/*==========================================================================*/
/*  wing.h  --  WinG High-Performance Graphics Library, version 1.0.         */
/*                                                                           */
/*  WinG, Copyright (C) Microsoft Corp. 1993-1994.                           */
/*                                                                           */
/*  WinG shipped in exactly one version, 1.0 (WING32.DLL FILEVERSION         */
/*  1.0.0.37, "WinG Version 1.0", 1994).  HEROES2W.EXE (Heroes of Might and  */
/*  Magic II "The Price of Loyalty", NWC 1996) links WING32.dll and imports  */
/*  6 functions BY NAME -- essentially the whole public API:                 */
/*      WinGCreateDC  WinGCreateBitmap  WinGRecommendDIBFormat               */
/*      WinGSetDIBColorTable  WinGStretchBlt  WinGBitBlt                     */
/*                                                                           */
/*  Retail codegen note: the WinG imports are reached via `call <thunk>` /   */
/*  `jmp dword ptr [__imp_]` (NOT the direct `call [IAT]` dllimport form the */
/*  Miles/Smacker imports use), so these are plain __stdcall externs with NO */
/*  __declspec(dllimport) -- matching the classic WING32.LIB import stubs.   */
/*                                                                           */
/*  This header is a RECONSTRUCTED copy of the documented WinG 1.0 API.  It  */
/*  supplies the minimal Win32/GDI types WinG needs when the canonical MSVC  */
/*  headers have not already supplied them, while remaining compatible with */
/*  translation units that include the full <windows.h>.                    */
/*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* ---- calling convention ------------------------------------------------- */
#ifndef WINAPI
#define WINAPI __stdcall
#endif
/* Linkage decoration on the return type. Empty here: the retail WinG imports
   are reached through classic import thunks, NOT __declspec(dllimport). */
#ifndef WINGAPI
#define WINGAPI
#endif
#ifndef FAR
#define FAR                 /* 32-bit flat model: no far pointers */
#endif

/* ---- minimal Win32 scalar aliases --------------------------------------- */
#if !defined(_WINDEF_) && !defined(_WINDEF_MINIMAL)
#define _WINDEF_MINIMAL
typedef int            BOOL;
typedef unsigned int   UINT;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long           LONG;
typedef unsigned char  BYTE;
typedef DWORD          COLORREF;
#endif

/* ---- opaque GDI handles (modelled as void*, like <win/windows.h>) ------- */
#if !defined(_WINGDI_) && !defined(_WINGDI_HANDLES_MINIMAL)
#define _WINGDI_HANDLES_MINIMAL
typedef void FAR* HDC;
typedef void FAR* HBITMAP;
typedef void FAR* HPALETTE;
typedef void FAR* HBRUSH;
#endif

/* ---- GDI DIB structures (standard Win32 layouts) ------------------------ */
#if !defined(_WINGDI_) && !defined(_WINGDI_DIB_MINIMAL)
#define _WINGDI_DIB_MINIMAL
typedef struct tagRGBQUAD {
   BYTE rgbBlue;
   BYTE rgbGreen;
   BYTE rgbRed;
   BYTE rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFOHEADER {
   DWORD biSize;
   LONG  biWidth;
   LONG  biHeight;
   WORD  biPlanes;
   WORD  biBitCount;
   DWORD biCompression;
   DWORD biSizeImage;
   LONG  biXPelsPerMeter;
   LONG  biYPelsPerMeter;
   DWORD biClrUsed;
   DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
   BITMAPINFOHEADER bmiHeader;
   RGBQUAD          bmiColors[1];
} BITMAPINFO;
#endif

/* ---- WinG halftone dither mode (WinG 1.0 dither kernels) ---------------- */
typedef enum {
   WING_DISPERSED_4x4,
   WING_DISPERSED_8x8,
   WING_CLUSTERED_4x4
} WING_DITHER_TYPE;

/*==========================================================================*/
/*  WinG 1.0 API.  (*) = imported by HEROES2W.EXE.                           */
/*==========================================================================*/
WINGAPI HDC       WINAPI WinGCreateDC(void);                                              /* (*) */
WINGAPI BOOL      WINAPI WinGRecommendDIBFormat(BITMAPINFO FAR* pFormat);                  /* (*) */
WINGAPI HBITMAP   WINAPI WinGCreateBitmap(HDC WinGDC,
                                          const BITMAPINFO FAR* pHeader,
                                          void FAR* FAR* ppBits);                          /* (*) */
WINGAPI void FAR* WINAPI WinGGetDIBPointer(HBITMAP WinGBitmap, BITMAPINFO FAR* pHeader);
WINGAPI UINT      WINAPI WinGGetDIBColorTable(HDC WinGDC, UINT StartIndex,
                                              UINT NumberOfEntries, RGBQUAD FAR* pColors);
WINGAPI UINT      WINAPI WinGSetDIBColorTable(HDC WinGDC, UINT StartIndex,
                                              UINT NumberOfEntries,
                                              const RGBQUAD FAR* pColors);                 /* (*) */
WINGAPI HPALETTE  WINAPI WinGCreateHalftonePalette(void);
WINGAPI HBRUSH    WINAPI WinGCreateHalftoneBrush(HDC Context, COLORREF Color,
                                                 WING_DITHER_TYPE DitherType);
WINGAPI BOOL      WINAPI WinGBitBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest,
                                    int nWidthDest, int nHeightDest,
                                    HDC hdcSrc, int nXOriginSrc, int nYOriginSrc);         /* (*) */
WINGAPI BOOL      WINAPI WinGStretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest,
                                        int nWidthDest, int nHeightDest,
                                        HDC hdcSrc, int nXOriginSrc, int nYOriginSrc,
                                        int nWidthSrc, int nHeightSrc);                    /* (*) */

#ifdef __cplusplus
}
#endif

#endif /* _INC_WING */
