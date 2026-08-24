#ifndef _INC_WING
#define _INC_WING


#ifdef __cplusplus
extern "C" {
#endif


#ifndef WINAPI
#define WINAPI __stdcall
#endif


#ifndef WINGAPI
#define WINGAPI
#endif
#ifndef FAR
#define FAR
#endif


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


#if !defined(_WINGDI_) && !defined(_WINGDI_HANDLES_MINIMAL)
#define _WINGDI_HANDLES_MINIMAL
typedef void FAR* HDC;
typedef void FAR* HBITMAP;
typedef void FAR* HPALETTE;
typedef void FAR* HBRUSH;
#endif


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


typedef enum {
   WING_DISPERSED_4x4,
   WING_DISPERSED_8x8,
   WING_CLUSTERED_4x4
} WING_DITHER_TYPE;


WINGAPI HDC       WINAPI WinGCreateDC(void);
WINGAPI BOOL      WINAPI WinGRecommendDIBFormat(BITMAPINFO FAR* pFormat);
WINGAPI HBITMAP   WINAPI WinGCreateBitmap(HDC WinGDC,
                                          const BITMAPINFO FAR* pHeader,
                                          void FAR* FAR* ppBits);
WINGAPI void FAR* WINAPI WinGGetDIBPointer(HBITMAP WinGBitmap, BITMAPINFO FAR* pHeader);
WINGAPI UINT      WINAPI WinGGetDIBColorTable(HDC WinGDC, UINT StartIndex,
                                              UINT NumberOfEntries, RGBQUAD FAR* pColors);
WINGAPI UINT      WINAPI WinGSetDIBColorTable(HDC WinGDC, UINT StartIndex,
                                              UINT NumberOfEntries,
                                              const RGBQUAD FAR* pColors);
WINGAPI HPALETTE  WINAPI WinGCreateHalftonePalette(void);
WINGAPI HBRUSH    WINAPI WinGCreateHalftoneBrush(HDC Context, COLORREF Color,
                                                 WING_DITHER_TYPE DitherType);
WINGAPI BOOL      WINAPI WinGBitBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest,
                                    int nWidthDest, int nHeightDest,
                                    HDC hdcSrc, int nXOriginSrc, int nYOriginSrc);
WINGAPI BOOL      WINAPI WinGStretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest,
                                        int nWidthDest, int nHeightDest,
                                        HDC hdcSrc, int nXOriginSrc, int nYOriginSrc,
                                        int nWidthSrc, int nHeightSrc);

#ifdef __cplusplus
}
#endif

#endif
