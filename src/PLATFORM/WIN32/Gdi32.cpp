#include <windows.h>

#include <cstring>

#include <PLATFORM/Platform.h>

#include "State.h"

namespace {

struct Palette {
    platform::Color entries[256] = {};
};

Palette gPalette;
int gPaletteToken = 0;

void PushPalette() {
    if (platform::win32::EnsureDisplay()) {
        platform::Video().SetPalette(gPalette.entries, 0, 256);
    }
}

}

HPALETTE CreatePalette(const LOGPALETTE* palette) {
    if (palette == nullptr) {
        return nullptr;
    }
    const WORD count = palette->palNumEntries > 256 ? 256 : palette->palNumEntries;
    for (WORD i = 0; i < count; ++i) {
        gPalette.entries[i].r = palette->palPalEntry[i].peRed;
        gPalette.entries[i].g = palette->palPalEntry[i].peGreen;
        gPalette.entries[i].b = palette->palPalEntry[i].peBlue;
    }
    return &gPaletteToken;
}

HPALETTE SelectPalette(HDC, HPALETTE palette, BOOL) { return palette; }

UINT RealizePalette(HDC) {
    PushPalette();
    return 256;
}

BOOL AnimatePalette(HPALETTE, UINT start, UINT count, const PALETTEENTRY* entries) {
    if (entries == nullptr) {
        return FALSE;
    }
    for (UINT i = 0; i < count; ++i) {
        const UINT slot = start + i;
        if (slot >= 256) {
            break;
        }
        gPalette.entries[slot].r = entries[i].peRed;
        gPalette.entries[slot].g = entries[i].peGreen;
        gPalette.entries[slot].b = entries[i].peBlue;
    }
    PushPalette();
    return TRUE;
}

UINT GetSystemPaletteEntries(HDC, UINT start, UINT count, LPPALETTEENTRY entries) {
    if (entries == nullptr) {
        return 0;
    }

    UINT written = 0;
    for (UINT i = 0; i < count && (start + i) < 256; ++i, ++written) {
        entries[i].peRed = gPalette.entries[start + i].r;
        entries[i].peGreen = gPalette.entries[start + i].g;
        entries[i].peBlue = gPalette.entries[start + i].b;
        entries[i].peFlags = 0;
    }
    return written;
}

HBITMAP CreateBitmapIndirect(const BITMAP* bitmap) {
    if (bitmap == nullptr) {
        return nullptr;
    }
    BITMAP* copy = new BITMAP(*bitmap);
    platform::win32::TrackGdiObject(copy);
    return copy;
}

HGDIOBJ SelectObject(HDC, HGDIOBJ object) { return object; }

BOOL DeleteObject(HGDIOBJ object) {
    if (object == nullptr || !platform::win32::ForgetGdiObject(object)) {
        return TRUE;
    }
    delete static_cast<BITMAP*>(object);
    return TRUE;
}
BOOL DeleteDC(HDC) { return TRUE; }

int GetDeviceCaps(HDC, int index) {
    switch (index) {
    case BITSPIXEL:

        return 8;
    case PLANES:
        return 1;
    case RASTERCAPS:
        return 0;
    default:
        return 0;
    }
}

BOOL PatBlt(HDC, int, int, int, int, DWORD) { return TRUE; }
DWORD GdiSetBatchLimit(DWORD limit) { return limit; }
