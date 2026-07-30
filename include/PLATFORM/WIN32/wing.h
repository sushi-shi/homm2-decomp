#ifndef HOMM2_PLATFORM_WIN32_WING_H
#define HOMM2_PLATFORM_WIN32_WING_H

#include <windows.h>

HDC WinGCreateDC();

BOOL WinGRecommendDIBFormat(BITMAPINFO* format);

HBITMAP WinGCreateBitmap(HDC dc, BITMAPINFO* header, void** bits);

UINT WinGSetDIBColorTable(HDC dc, UINT start, UINT count, RGBQUAD* colors);

BOOL WinGBitBlt(HDC destination, int x, int y, int width, int height,
                HDC source, int sourceX, int sourceY);
BOOL WinGStretchBlt(HDC destination, int x, int y, int width, int height,
                    HDC source, int sourceX, int sourceY, int sourceWidth,
                    int sourceHeight);

#endif
