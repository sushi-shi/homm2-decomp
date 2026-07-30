#include <windows.h>
#include <wing.h>

#include <cstring>

#include <PLATFORM/Platform.h>

#include "State.h"

namespace {

int gMemoryDeviceContext = 0;

}

HDC WinGCreateDC() {
    if (!platform::win32::EnsureDisplay()) {
        return nullptr;
    }
    return &gMemoryDeviceContext;
}

BOOL WinGRecommendDIBFormat(BITMAPINFO* format) {
    if (format == nullptr) {
        return FALSE;
    }

    std::memset(&format->bmiHeader, 0, sizeof(format->bmiHeader));
    format->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    format->bmiHeader.biPlanes = 1;
    format->bmiHeader.biBitCount = 8;
    format->bmiHeader.biCompression = BI_RGB;
    format->bmiHeader.biWidth = platform::Video().Resolution().width;
    format->bmiHeader.biHeight = -platform::Video().Resolution().height;
    return TRUE;
}

HBITMAP WinGCreateBitmap(HDC dc, BITMAPINFO* header, void** bits) {
    if (bits == nullptr) {
        return nullptr;
    }
    if (!platform::win32::EnsureDisplay()) {
        return nullptr;
    }

    *bits = platform::Video().Pixels();

    if (header != nullptr) {
        header->bmiHeader.biWidth = platform::Video().Resolution().width;
        header->bmiHeader.biHeight = -platform::Video().Resolution().height;
        header->bmiHeader.biBitCount = 8;
        header->bmiHeader.biPlanes = 1;
    }
    return &gMemoryDeviceContext;
}

UINT WinGSetDIBColorTable(HDC dc, UINT start, UINT count, RGBQUAD* colors) {
    if (colors == nullptr || count == 0) {
        return 0;
    }

    platform::Color entries[256];
    const UINT limit = count > 256 ? 256 : count;
    for (UINT i = 0; i < limit; ++i) {
        entries[i].r = colors[i].rgbRed;
        entries[i].g = colors[i].rgbGreen;
        entries[i].b = colors[i].rgbBlue;
    }
    platform::Video().SetPalette(entries, static_cast<int>(start), static_cast<int>(limit));
    return limit;
}

BOOL WinGBitBlt(HDC destination, int x, int y, int width, int height, HDC source, int sourceX,
                int sourceY) {

    platform::Rect region;
    region.x = sourceX;
    region.y = sourceY;
    region.width = width;
    region.height = height;
    platform::Video().Blit(region, platform::Point{x, y});
    platform::Video().Present();
    return TRUE;
}

BOOL WinGStretchBlt(HDC destination, int x, int y, int width, int height, HDC source, int sourceX,
                    int sourceY, int sourceWidth, int sourceHeight) {

    platform::Rect region;
    region.x = sourceX;
    region.y = sourceY;
    region.width = sourceWidth > 0 ? sourceWidth : width;
    region.height = sourceHeight > 0 ? sourceHeight : height;
    platform::Video().Blit(region, platform::Point{x, y});
    platform::Video().Present();
    return TRUE;
}
