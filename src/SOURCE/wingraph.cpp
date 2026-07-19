#include <stdio.h>
#include <string.h>
#include <va.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <BASE/palette.h>
#include <BASE/INPUTMGR.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/wingraph.h>

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\wingraph.cpp"

DATA(0x00525148) static RECT gDDSourceRect;
DATA(0x00525158) static RECT gDDClientRect;
DATA(0x00525170) static RECT gDDDestinationRect;
DATA(0x005255ac) static HRESULT gDDResult;
DATA(0x005255b0) static DDSURFACEDESC gDDSurfaceDesc;
DATA(0x004f16c4) static i16 gDDRestoreLineBase = 45;
DATA(0x004f16f4) static i16 gCreatePrimaryLineBase = 67;
DATA(0x004f1724) static i16 gSetupClipperLineBase = 87;
DATA(0x004f17ac) static i16 gDDInitLineBase = 110;
DATA(0x004f1864) static i16 gDDPaintLineBase = 157;
DATA(0x004f19b0) static i16 gDDInitializePaletteLineBase = 296;
DATA(0x004f19e0) static i16 gDDSetPaletteLineBase = 368;
DATA(0x004f1a10) static i16 gDDCreateSurfaceLineBase = 393;
DATA(0x004f1dbc) static i16 gDDUpdatePaletteLineBase = 500;
DATA(0x004f1e18) static i16 gDDCleanUpLineBase = 526;
DATA(0x004f1e74) static i16 gDDSetFullScreenLineBase = 572;
DATA(0x004f2084) static i16 gSetGraphicsTypeLineBase = 1247;

VA(0x00435290, 0x55)
void DDRestoreDisplayMode(void) {
    HRESULT result;

    if (lpDD != NULL) {
        result = lpDD->RestoreDisplayMode();
        if (result != DD_OK)
            DDSD(result, RETAIL_FILE, 52);
    }
}

VA(0x004352e5, 0x46)
i32 DDQueryNewPalette(void) {
    i32 unused;

    if (gbWinGraphBusy != 0)
        return 1;
    if (gbForegroundApp == 0)
        return 1;
    return SetPalette();
}

VA(0x0043532b, 0x94)
void CreatePrimary(void) {
    HRESULT result;

    lpDDSPrimary = DDCreateSurface(WINGRAPH_WIDTH, WINGRAPH_HEIGHT, 1);
    if (lpClipper != NULL) {
        result = lpDDSPrimary->SetClipper(NULL);
        if (result != DD_OK && result != DDERR_NOCLIPPERATTACHED)
            DDSD(result, RETAIL_FILE, 77);
        lpClipper->Release();
        lpClipper = NULL;
    }
}

VA(0x004353bf, 0xe3)
void SetupClipper(void) {
    HRESULT result;

    if (gConfig.gfx[giCurExe].fullScreen == 0) {
        result = lpDD->CreateClipper(0, &lpClipper, NULL);
        if (result != DD_OK)
            DDSD(result, RETAIL_FILE, 95);
        result = lpClipper->SetHWnd(0, hwndApp);
        if (result != DD_OK)
            DDSD(result, RETAIL_FILE, 100);
        result = lpDDSPrimary->SetClipper(lpClipper);
        if (result != DD_OK)
            DDSD(result, RETAIL_FILE, 105);
    }
}

VA(0x004354a2, 0x15f)
void DDInitGraphics(void) {
    HRESULT result;

    if (gbWinGraphBusy != 0)
        return;
    result = lpDirectDrawCreate(NULL, &lpDD, NULL);
    if (result != DD_OK)
        DDSD(result, RETAIL_FILE, 118);
    if (gConfig.gfx[giCurExe].fullScreen != 0) {
        SetMenuStatus(0);
        result = lpDD->SetCooperativeLevel(
            hwndApp,
            DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT
        );
        if (result != DD_OK)
            DDSD(result, RETAIL_FILE, 130);
        result = lpDD->SetDisplayMode(WINGRAPH_WIDTH, WINGRAPH_HEIGHT, WINGRAPH_COLOR_DEPTH);
        if (result != DD_OK)
            DDSD(result, RETAIL_FILE, 134);
    } else {
        result = lpDD->SetCooperativeLevel(hwndApp, DDSCL_NORMAL);
        if (result != DD_OK)
            DDSD(result, RETAIL_FILE, 141);
    }
    CreatePrimary();
    SetupClipper();
    lpDDSOne = DDCreateSurface(WINGRAPH_WIDTH, WINGRAPH_HEIGHT, 0);
    InitializePalette();
}

VA(0x00435601, 0x530)
i32 DDAppPaint(void* window, void* paintDC) {
    i32 sourceTop4;
    i32 sourceWidth5;
    i32 sourceHeight3;
    i32 sourceLeft3;
    POINT point0;
    PAINTSTRUCT paint3;

    if (gbWinGraphBusy != 0)
        return 1;
    if (gbMinimized != 0)
        return 1;
    if (lpDD == NULL)
        return 1;
    {
        gbWinGraphBusy = true;
        paintDC = BeginPaint(reinterpret_cast<HWND>(window), &paint3);
        GetClientRect(reinterpret_cast<HWND>(window), &gDDClientRect);
        if (paint3.rcPaint.right == 0 || paint3.rcPaint.bottom == 0)
            paint3.rcPaint = gDDClientRect;
        if (paint3.rcPaint.right < WINGRAPH_WIDTH)
            paint3.rcPaint.right++;
        if (paint3.rcPaint.bottom < WINGRAPH_HEIGHT)
            paint3.rcPaint.bottom++;

        gDDDestinationRect = paint3.rcPaint;
        sourceWidth5 = ((gDDDestinationRect.right - gDDDestinationRect.left + 1) * WINGRAPH_WIDTH)
                       / iMainWinScreenWidth;
        sourceHeight3 = ((gDDDestinationRect.bottom - gDDDestinationRect.top + 1) * WINGRAPH_HEIGHT)
                        / iMainWinScreenHeight;
        sourceLeft3 = (gDDDestinationRect.left * WINGRAPH_WIDTH) / iMainWinScreenWidth;
        sourceTop4 = (gDDDestinationRect.top * WINGRAPH_HEIGHT) / iMainWinScreenHeight;
        if (giScrollX != 0) {
            sourceLeft3 = giScrollX + WINGRAPH_SCROLL_MARGIN;
            sourceWidth5 = WINGRAPH_SCROLL_SIZE;
        }
        if (giScrollY != 0) {
            sourceTop4 = giScrollY + WINGRAPH_SCROLL_MARGIN;
            sourceHeight3 = WINGRAPH_SCROLL_SIZE;
        }
        gDDSourceRect.left = sourceLeft3;
        gDDSourceRect.right = OD_STEER(sourceWidth5) + sourceLeft3 - 1;
        gDDSourceRect.top = sourceTop4;
        gDDSourceRect.bottom = sourceTop4 + sourceHeight3 - 1;

        point0.y = 0;
        point0.x = point0.y;
        ClientToScreen(hwndApp, &point0);
        OffsetRect(&gDDDestinationRect, point0.x, point0.y);
        gDDResult = lpDDSOne->Unlock(NULL);
        if (gDDResult != DD_OK)
            DDSD(gDDResult, RETAIL_FILE, 228);

        if (gDDSourceRect.left < 0)
            gDDSourceRect.left = 0;
        if (gDDSourceRect.top < 0)
            gDDSourceRect.top = 0;
        if (gDDSourceRect.right > WINGRAPH_WIDTH)
            gDDSourceRect.right = WINGRAPH_WIDTH;
        if (gDDSourceRect.bottom > WINGRAPH_HEIGHT)
            gDDSourceRect.bottom = WINGRAPH_HEIGHT;

        lPaintStart = KBTickCount();
        for (;;) {
            gDDResult =
                lpDDSPrimary->Blt(&gDDDestinationRect, lpDDSOne, &gDDSourceRect, DDBLT_WAIT, NULL);
            if (gDDResult == DDERR_SURFACELOST) {
                gDDResult = lpDDSPrimary->Restore();
                if (gDDResult == DDERR_WRONGMODE) {
                    LogStr("ResetDisplayMode");
                    gDDResult =
                        lpDD->SetDisplayMode(WINGRAPH_WIDTH, WINGRAPH_HEIGHT, WINGRAPH_COLOR_DEPTH);
                    if (gDDResult != DD_OK)
                        DDSD(gDDResult, RETAIL_FILE, 248);
                    gDDResult = lpDDSPrimary->Restore();
                    if (gDDResult != DD_OK)
                        DDSD(gDDResult, RETAIL_FILE, 252);
                    gDDDestinationRect = gDDSourceRect;
                }
                if (gDDResult != DD_OK)
                    DDSD(gDDResult, RETAIL_FILE, 260);
            } else if (gDDResult == DDERR_SURFACEBUSY
                       && KBTickCount() < lPaintStart + WINGRAPH_PAINT_TIMEOUT) {
                iBusyRetry++;
            } else if (gDDResult != DD_OK) {
                DDSD(gDDResult, RETAIL_FILE, 265);
            } else {
                break;
            }
        }

        memset(&gDDSurfaceDesc, 0, sizeof(gDDSurfaceDesc));
        gDDSurfaceDesc.dwSize = sizeof(gDDSurfaceDesc);
        gDDResult = lpDDSOne->Lock(NULL, &gDDSurfaceDesc, DDLOCK_WAIT, NULL);
        if (gDDResult != DD_OK)
            DDSD(gDDResult, RETAIL_FILE, 275);
        if (gpWindowManager->m_screen != NULL) {
            gpWindowManager->m_screen->m_pixels = static_cast<u8*>(gDDSurfaceDesc.lpSurface);
            lpInitWin = gDDSurfaceDesc.lpSurface;
        } else {
            lpInitWin = gDDSurfaceDesc.lpSurface;
        }
        if (gDDResult != DD_OK)
            DDSD(gDDResult, RETAIL_FILE, 286);
        EndPaint(reinterpret_cast<HWND>(window), &paint3);
        gbWinGraphBusy = false;
    }
    return 1;
}

VA(0x00435b31, 0x13c)
void DDInitializePalette(void) {
    HDC dc0;
    i32 entry;
    HRESULT result0;

    if (gbWinGraphBusy != 0)
        return;
    {
        dc0 = GetDC(NULL);
        GetSystemPaletteEntries(dc0, 0, WINGRAPH_SYSTEM_PALETTE_SIZE, LogicalPalette.entries);
        GetSystemPaletteEntries(
            dc0,
            WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE,
            WINGRAPH_SYSTEM_PALETTE_SIZE,
            &LogicalPalette.entries[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE]
        );
        ReleaseDC(NULL, dc0);
        for (entry = 0; entry < WINGRAPH_SYSTEM_PALETTE_SIZE; entry++) {
            LogicalPalette.entries[entry].peFlags = 0;
            LogicalPalette.entries[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry]
                .peFlags = 0;
        }
        for (entry = WINGRAPH_SYSTEM_PALETTE_SIZE;
             entry < WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE;
             entry++) {
            LogicalPalette.entries[entry].peRed = 0;
            LogicalPalette.entries[entry].peGreen = 0;
            LogicalPalette.entries[entry].peBlue = 0;
            LogicalPalette.entries[entry].peFlags = PC_NOCOLLAPSE;
        }
        result0 = lpDD->CreatePalette(DDPCAPS_8BIT, LogicalPalette.entries, &lpDDPal, NULL);
        if (result0 != DD_OK)
            DDSD(result0, RETAIL_FILE, 359);
        SetPalette();
    }
}

VA(0x00435c6d, 0xaf)
i32 DDSetPalette(void) {
    HRESULT result;

    if (gbWinGraphBusy != 0)
        return 1;
    if (gbForegroundApp == 0)
        return 1;
    if (lpDDPal == NULL || lpDDSPrimary == NULL || lpDD == NULL)
        return 1;
    result = lpDDSPrimary->SetPalette(lpDDPal);
    if (result != DD_OK)
        DDSD(result, RETAIL_FILE, 383);
    return 0;
}

VA(0x00435d1c, 0x133)
struct IDirectDrawSurface* DDCreateSurface(u32l width, u32l height, i32 primary) {
    DDSURFACEDESC description;
    IDirectDrawSurface* surface;
    i32 cnt;
    i32 unused;
    HRESULT rv;

    memset(&description, 0, sizeof(description));
    description.dwSize = sizeof(description);
    if (primary != 0) {
        description.dwFlags = DDSD_CAPS;
        description.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    } else {
        description.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        description.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        description.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
        description.dwHeight = height;
        description.dwWidth = width;
    }
    rv = lpDD->CreateSurface(&description, &surface, NULL);
    if (rv != DD_OK)
        DDSD(rv, RETAIL_FILE, 421);
    if (primary == 0) {
        rv = surface->Lock(NULL, &description, DDLOCK_WAIT, NULL);
        if (rv != DD_OK)
            DDSD(rv, RETAIL_FILE, 429);
        if (gpWindowManager->m_screen != NULL) {
            gpWindowManager->m_screen->m_pixels = static_cast<u8*>(description.lpSurface);
            lpInitWin = description.lpSurface;
        } else {
            lpInitWin = description.lpSurface;
        }
    }
    return surface;
}

VA(0x00435e4f, 0x5d2)
void DDSD(i32 error, char* file, i32 line) {
    HRESULT restoreResult;
    i32 unused;
    char errorMessage0[WINGRAPH_ERROR_TEXT_SIZE];

    if (bInDDSD != 0)
        return;
    bInDDSD = 1;
    restoreResult = lpDD->RestoreDisplayMode();
    unused = 0;
    switch (error) {
        case DD_OK:
            return;
        case DDERR_GENERIC:
            strcpy(errorMessage0, "DDERR_GENERIC            ");
            break;
        case DDERR_INVALIDCLIPLIST:
            strcpy(errorMessage0, "DDERR_INVALIDCLIPLIST\t   ");
            break;
        case DDERR_INVALIDOBJECT:
            strcpy(errorMessage0, "DDERR_INVALIDOBJECT      ");
            break;
        case DDERR_INVALIDPARAMS:
            strcpy(errorMessage0, "DDERR_INVALIDPARAMS\t   ");
            break;
        case DDERR_INVALIDRECT:
            strcpy(errorMessage0, "DDERR_INVALIDRECT\t      ");
            break;
        case DDERR_NOALPHAHW:
            strcpy(errorMessage0, "DDERR_NOALPHAHW          ");
            break;
        case DDERR_NOBLTHW:
            strcpy(errorMessage0, "DDERR_NOBLTHW\t         ");
            break;
        case DDERR_NOCLIPLIST:
            strcpy(errorMessage0, "DDERR_NOCLIPLIST         ");
            break;
        case DDERR_NODDROPSHW:
            strcpy(errorMessage0, "DDERR_NODDROPSHW\t      ");
            break;
        case DDERR_SURFACELOST:
            strcpy(errorMessage0, "DDERR_SURFACELOST        ");
            break;
        case DDERR_UNSUPPORTED:
            strcpy(errorMessage0, "DDERR_UNSUPPORTED\t      ");
            break;
        case DDERR_NOMIRRORHW:
            strcpy(errorMessage0, "DDERR_NOMIRRORHW         ");
            break;
        case DDERR_NORASTEROPHW:
            strcpy(errorMessage0, "DDERR_NORASTEROPHW\t      ");
            break;
        case DDERR_NOROTATIONHW:
            strcpy(errorMessage0, "DDERR_NOROTATIONHW       ");
            break;
        case DDERR_NOSTRETCHHW:
            strcpy(errorMessage0, "DDERR_NOSTRETCHHW\t      ");
            break;
        case DDERR_SURFACEBUSY:
            strcpy(errorMessage0, "DDERR_SURFACEBUSY        ");
            break;
        case DDERR_NOZBUFFERHW:
            strcpy(errorMessage0, "DDERR_NOZBUFFERHW        ");
            break;
        case DDERR_OUTOFMEMORY:
            strcpy(errorMessage0, "DDERR_OUTOFMEMORY\t      ");
            break;
        case DDERR_CLIPPERISUSINGHWND:
            strcpy(errorMessage0, "DDERR_CLIPPERISUSINGHWND ");
            break;
        case DDERR_NOEXCLUSIVEMODE:
            strcpy(errorMessage0, "DDERR_NOEXCLUSIVEMODE\t   ");
            break;
        case DDERR_NOT8BITCOLOR:
            strcpy(errorMessage0, "DDERR_NOT8BITCOLOR       ");
            break;
        case DDERR_NOPALETTEATTACHED:
            strcpy(errorMessage0, "DDERR_NOPALETTEATTACHED  ");
            break;
        case DDERR_NOPALETTEHW:
            strcpy(errorMessage0, "DDERR_NOPALETTEHW\t      ");
            break;
        case DDERR_LOCKEDSURFACES:
            strcpy(errorMessage0, "DDERR_LOCKEDSURFACES     ");
            break;
        case DDERR_IMPLICITLYCREATED:
            strcpy(errorMessage0, "DDERR_IMPLICITLYCREATED  ");
            break;
        case DDERR_WRONGMODE:
            strcpy(errorMessage0, "DDERR_WRONGMODE          ");
            break;
        case DDERR_INCOMPATIBLEPRIMARY:
            strcpy(errorMessage0, "DDERR_INCOMPATIBLEPRIMARY");
            break;
        case DDERR_NOCLIPPERATTACHED:
            strcpy(errorMessage0, "DDERR_NOCLIPPERATTACHED  ");
            break;
        default:
            strcpy(errorMessage0, "Error type unknown");
            break;
    }
    MessageBeep(0);
    MessageBeep(0);
    MessageBeep(0);
    sprintf(
        gText,
        "DirectDraw Error:\n\n  '%s'\n\n  File: '%s'\n  Line: %d",
        errorMessage0,
        file,
        line
    );
    LogStr(gText);
    ShutDown(gText);
}

VA(0x00436421, 0x118)
void DDUpdatePalette(i8* paletteData) {
    i32 entry;
    HRESULT result0;

    if (gbWinGraphBusy != 0)
        return;
    if (gbForegroundApp == 0)
        return;
    for (entry = WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry < WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry++) {
        LogicalPalette.entries[entry].peRed = paletteData[entry * 3] << 2;
        LogicalPalette.entries[entry].peGreen = paletteData[entry * 3 + 1] << 2;
        LogicalPalette.entries[entry].peBlue = paletteData[entry * 3 + 2] << 2;
        LogicalPalette.entries[entry].peFlags = PC_NOCOLLAPSE;
    }
    ProcessAssert(reinterpret_cast<i32>(lpDDPal), RETAIL_FILE, 518);
    result0 = lpDDPal->SetEntries(
        0,
        WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE * 2,
        &LogicalPalette.entries[WINGRAPH_SYSTEM_PALETTE_SIZE]
    );
    if (result0 != DD_OK)
        DDSD(result0, RETAIL_FILE, 522);
}

VA(0x00436539, 0x177)
void DDCleanUpWinGraphics(void) {
    HRESULT restoreResult;
    HRESULT result;

    if (lpDD != NULL) {
        restoreResult = lpDD->RestoreDisplayMode();
        if (lpClipper != NULL) {
            if (lpDDSPrimary != NULL) {
                result = lpDDSPrimary->SetClipper(NULL);
                if (result != DD_OK && result != DDERR_NOCLIPPERATTACHED)
                    DDSD(result, RETAIL_FILE, 540);
            }
            lpClipper->Release();
            lpClipper = NULL;
        }
        if (lpDDSPrimary != NULL) {
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        }
        if (lpDDSOne != NULL) {
            lpDDSOne->Release();
            lpDDSOne = NULL;
        }
        if (lpDDPal != NULL) {
            lpDDPal->Release();
            lpDDPal = NULL;
        }
        result = lpDD->SetCooperativeLevel(hwndApp, DDSCL_NORMAL);
        if (result != DD_OK)
            DDSD(result, RETAIL_FILE, 564);
        lpDD->Release();
        lpDD = NULL;
    }
}

VA(0x004366b0, 0x305)
void DDSetFullScreenStatus(i32 fullScreen) {
    i32 width;
    i32 windowHeight0;
    i32 x;
    i32 y;
    HRESULT result0;

    if (gbWinGraphBusy != 0)
        return;
    if (gConfig.gfx[giCurExe].fullScreen == fullScreen)
        return;
    {
        x = gConfig.gfx[giCurExe].x;
        y = gConfig.gfx[giCurExe].y;
        width = gConfig.gfx[giCurExe].width;
        windowHeight0 = gConfig.gfx[giCurExe].height;
        gbWinGraphBusy = true;
        gConfig.gfx[giCurExe].fullScreen = fullScreen;
        if (gConfig.gfx[giCurExe].fullScreen != 0)
            SetMenuStatus(0);

        result0 = lpDD->SetCooperativeLevel(
            hwndApp,
            DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT
        );
        if (result0 != DD_OK)
            DDSD(result0, RETAIL_FILE, 593);
        if (gConfig.gfx[giCurExe].fullScreen != 0) {
            result0 = lpDD->SetDisplayMode(WINGRAPH_WIDTH, WINGRAPH_HEIGHT, WINGRAPH_COLOR_DEPTH);
            if (result0 != DD_OK)
                DDSD(result0, RETAIL_FILE, 599);
        } else {
            result0 = lpDD->RestoreDisplayMode();
            if (result0 != DD_OK)
                DDSD(result0, RETAIL_FILE, 606);
            result0 = lpDD->SetCooperativeLevel(hwndApp, DDSCL_NORMAL);
            if (result0 != DD_OK)
                DDSD(result0, RETAIL_FILE, 611);
        }
        if (lpDDSPrimary != NULL) {
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        }
        CreatePrimary();
        result0 = lpDDSPrimary->SetPalette(lpDDPal);
        if (result0 != DD_OK)
            DDSD(result0, RETAIL_FILE, 623);
        WritePrefs();
        gbWinGraphBusy = false;
        if (gConfig.gfx[giCurExe].fullScreen == 0) {
            SetMenuStatus(1);
            ResizeWindow(x, y, width, windowHeight0);
        } else {
            gConfig.gfx[giCurExe].x = x;
            gConfig.gfx[giCurExe].y = y;
            gConfig.gfx[giCurExe].width = width;
            gConfig.gfx[giCurExe].height = windowHeight0;
        }
        SetupClipper();
    }
}

VA(0x004369b5, 0x89)
i32 WGQueryNewPalette(void) {
    i32 paletteChanges;
    HDC hdc;

    hdc = GetDC(hwndApp);
    if (hpalApp != NULL)
        SelectPalette(hdc, hpalApp, 0);
    paletteChanges = RealizePalette(hdc);
    ReleaseDC(hwndApp, hdc);
    if (paletteChanges > 0) {
        InvalidateRect(hwndApp, NULL, 1);
        return 1;
    } else {
        return 0;
    }
}

VA(0x00436a3e, 0x151)
void WGInitGraphics(void) {
    HBITMAP bitmap;

    if (hdcImage != NULL)
        return;
    if (WinGRecommendDIBFormat(reinterpret_cast<BITMAPINFO*>(&screenImage)) != 0) {
        screenImage.header.biBitCount = WINGRAPH_COLOR_DEPTH;
        screenImage.header.biCompression = BI_RGB;
        Orientation = screenImage.header.biHeight;
    } else {
        screenImage.header.biSize = sizeof(BITMAPINFOHEADER);
        screenImage.header.biPlanes = 1;
        screenImage.header.biBitCount = WINGRAPH_COLOR_DEPTH;
        screenImage.header.biCompression = BI_RGB;
        screenImage.header.biSizeImage = 0;
        screenImage.header.biClrUsed = 0;
        screenImage.header.biClrImportant = 0;
    }
    screenImage.header.biWidth = WINGRAPH_WIDTH;
    screenImage.header.biHeight = -WINGRAPH_HEIGHT;
    InitializePalette();
    hdcImage = WinGCreateDC();
    screenImage.header.biWidth = WINGRAPH_WIDTH;
    screenImage.header.biHeight = -WINGRAPH_HEIGHT;
    bitmap =
        WinGCreateBitmap(hdcImage, reinterpret_cast<BITMAPINFO*>(&screenImage), &screenImage.bits);
    screenImage.header.biSizeImage = (screenImage.header.biWidth | 0) * screenImage.header.biHeight;
    screenImage.header.biSizeImage *= Orientation;
    gbmOldMonoBitmap = static_cast<HBITMAP>(SelectObject(hdcImage, bitmap));
    lpInitWin = screenImage.bits;
    PatBlt(hdcImage, 0, 0, iMainWinScreenWidth, iMainWinScreenHeight, BLACKNESS);
}

VA(0x00436b8f, 0x1e7)
void WGUpdatePalette(i8* paletteData) {
    HDC dc0;
    i32 entry;
    i32 result0;

    for (entry = WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry < WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry++) {
        LogicalPalette.entries[entry].peRed = paletteData[entry * 3] << 2;
        screenImage.colors[entry].rgbRed = LogicalPalette.entries[entry].peRed;
        LogicalPalette.entries[entry].peGreen = paletteData[entry * 3 + 1] << 2;
        screenImage.colors[entry].rgbGreen = LogicalPalette.entries[entry].peGreen;
        LogicalPalette.entries[entry].peBlue = paletteData[entry * 3 + 2] << 2;
        screenImage.colors[entry].rgbBlue = LogicalPalette.entries[entry].peBlue;
    }
    AnimatePalette(
        hpalApp,
        WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE * 2,
        &LogicalPalette.entries[WINGRAPH_SYSTEM_PALETTE_SIZE]
    );
    WinGSetDIBColorTable(
        hdcImage,
        WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE * 2,
        &screenImage.colors[WINGRAPH_SYSTEM_PALETTE_SIZE]
    );
    if (hpalApp != NULL)
        DeleteObject(hpalApp);
    hpalApp = CreatePalette(reinterpret_cast<LOGPALETTE*>(&LogicalPalette));
    dc0 = GetDC(hwndApp);
    if (hpalApp != NULL)
        SelectPalette(dc0, hpalApp, 0);
    result0 = RealizePalette(dc0);
    ReleaseDC(hwndApp, dc0);
    if (giMainVideoModeColorDepth != WINGRAPH_COLOR_DEPTH && gpWindowManager->m_screen != NULL) {
        if (gbLimitedCombatUpdatePalette != 0) {
            if (gbFullCombatScreenDrawn != 0)
                BlitBitmapToScreen(
                    gpWindowManager->m_screen,
                    0,
                    0,
                    WINGRAPH_WIDTH,
                    WINGRAPH_LIMITED_COMBAT_HEIGHT,
                    0,
                    0
                );
        } else {
            BlitBitmapToScreen(
                gpWindowManager->m_screen,
                0,
                0,
                WINGRAPH_WIDTH,
                WINGRAPH_HEIGHT,
                0,
                0
            );
        }
    }
}

VA(0x00436d76, 0x1d5)
void WGInitializePalette(void) {
    HDC dc0;
    i32 entry;

    if (hpalApp != NULL)
        return;
    dc0 = GetDC(NULL);
    GetSystemPaletteEntries(dc0, 0, WINGRAPH_SYSTEM_PALETTE_SIZE, LogicalPalette.entries);
    GetSystemPaletteEntries(
        dc0,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_SYSTEM_PALETTE_SIZE,
        &LogicalPalette.entries[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE]
    );
    ReleaseDC(NULL, dc0);
    for (entry = 0; entry < WINGRAPH_SYSTEM_PALETTE_SIZE; entry++) {
        screenImage.colors[entry].rgbRed = LogicalPalette.entries[entry].peRed;
        screenImage.colors[entry].rgbGreen = LogicalPalette.entries[entry].peGreen;
        screenImage.colors[entry].rgbBlue = LogicalPalette.entries[entry].peBlue;
        screenImage.colors[entry].rgbReserved = 0;
        LogicalPalette.entries[entry].peFlags = 0;
        screenImage.colors[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry].rgbRed =
            LogicalPalette.entries[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry]
                .peRed;
        screenImage.colors[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry].rgbGreen =
            LogicalPalette.entries[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry]
                .peGreen;
        screenImage.colors[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry].rgbBlue =
            LogicalPalette.entries[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry]
                .peBlue;
        screenImage.colors[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry]
            .rgbReserved = 0;
        LogicalPalette.entries[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry]
            .peFlags = 0;
    }
    for (entry = WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry < WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry++) {
        LogicalPalette.entries[entry].peRed = 0;
        screenImage.colors[entry].rgbRed = LogicalPalette.entries[entry].peRed;
        LogicalPalette.entries[entry].peGreen = 0;
        screenImage.colors[entry].rgbGreen = LogicalPalette.entries[entry].peGreen;
        LogicalPalette.entries[entry].peBlue = 0;
        screenImage.colors[entry].rgbBlue = LogicalPalette.entries[entry].peBlue;
        screenImage.colors[entry].rgbReserved = 0;
        LogicalPalette.entries[entry].peFlags = PC_NOCOLLAPSE;
    }
    hpalApp = CreatePalette(reinterpret_cast<LOGPALETTE*>(&LogicalPalette));
}

VA(0x00436f4b, 0x1c4)
i32 WGAppPaint(void* window, void* paintDC) {
    i32 padding8;
    RECT clientRect16;
    char unusedByte0;
    i32 destY1;
    i32 sourceX7;
    PAINTSTRUCT paint5;
    i32 destHeight3;
    i32 destWidth0;
    i32 destX7;
    i32 sourceY7;

    unusedByte0 = 0;
    if (screenImage.bits != NULL) {
        paintDC = BeginPaint(reinterpret_cast<HWND>(window), &paint5);
        SelectPalette(static_cast<HDC>(paintDC), hpalApp, 0);
        RealizePalette(static_cast<HDC>(paintDC));
        GetClientRect(reinterpret_cast<HWND>(window), &clientRect16);
        destX7 = 0;
        sourceX7 = destX7;
        destY1 = 0;
        sourceY7 = destY1;
        destWidth0 = clientRect16.right - clientRect16.left;
        destHeight3 = clientRect16.bottom - clientRect16.top;
        sourceX7 = (destX7 * WINGRAPH_WIDTH) / iMainWinScreenWidth;
        sourceY7 = (destY1 * WINGRAPH_HEIGHT) / iMainWinScreenHeight;
        if (giScrollX != 0)
            sourceX7 += giScrollX;
        if (giScrollY != 0)
            sourceY7 += giScrollY;
        giTtlBlts++;
        if (iMainWinScreenWidth == WINGRAPH_WIDTH && iMainWinScreenHeight == WINGRAPH_HEIGHT) {
            destX7 = paint5.rcPaint.left & WINGRAPH_PAINT_ALIGN_MASK;
            destWidth0 = paint5.rcPaint.right - destX7 + 1;
            destY1 = paint5.rcPaint.top;
            destHeight3 = paint5.rcPaint.bottom - destY1 + 1;
            WinGBitBlt(
                static_cast<HDC>(paintDC),
                destX7,
                destY1,
                destWidth0,
                destHeight3,
                hdcImage,
                OD_STEER(giScrollX) + destX7,
                OD_STEER(giScrollY) + destY1
            );
        } else {
            WinGStretchBlt(
                static_cast<HDC>(paintDC),
                destX7,
                destY1,
                destWidth0,
                destHeight3,
                hdcImage,
                sourceX7,
                sourceY7,
                (destWidth0 * WINGRAPH_WIDTH) / iMainWinScreenWidth,
                (destHeight3 * WINGRAPH_HEIGHT) / iMainWinScreenHeight
            );
        }
        EndPaint(reinterpret_cast<HWND>(window), &paint5);
    }
    return 1;
}

VA(0x0043710f, 0x78)
void WGCleanUpWinGraphics(void) {
    HGDIOBJ bitmap;

    if (hdcImage != NULL) {
        bitmap = SelectObject(hdcImage, gbmOldMonoBitmap);
        DeleteObject(bitmap);
        DeleteDC(hdcImage);
        hdcImage = NULL;
    }
    if (hpalApp != NULL) {
        DeleteObject(hpalApp);
        hpalApp = NULL;
    }
}

VA(0x00437187, 0x80)
void ConnectToDLLs(void) {
    hDDrawLibrary = LoadLibraryA("DDRAW.DLL");
    if (reinterpret_cast<u32l>(hDDrawLibrary) >= WINGRAPH_LOAD_LIBRARY_SUCCESS) {
        lpDirectDrawCreate = reinterpret_cast<DirectDrawCreateProc>(
            GetProcAddress(hDDrawLibrary, "DirectDrawCreate")
        );
    }
    if (lpDirectDrawCreate != NULL) {
        gbDDrawAttached = true;
    } else {
        gConfig.gfx[giCurExe].fullScreen = 0;
        SetMenuStatus(1);
    }
}

VA(0x00437207, 0x29)
void DisconnectDLLs(void) {
    if (reinterpret_cast<u32l>(hDDrawLibrary) >= WINGRAPH_LOAD_LIBRARY_SUCCESS) {
        FreeLibrary(hDDrawLibrary);
    }
}

VA(0x00437230, 0x2c)
void RestoreDisplayMode(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING) {
        return;
    } else {
        DDRestoreDisplayMode();
    }
}

VA(0x0043725c, 0x2e)
i32 SetPalette(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        return 0;
    else
        return DDSetPalette();
}

VA(0x0043728a, 0x7e)
void GetGraphicsInfo(void) {
    HDC screenDC;

    screenDC = GetDC(NULL);
    if (screenDC != NULL) {
        giMainVideoModeColorDepth = GetDeviceCaps(screenDC, BITSPIXEL);
        giMainVideoModeWidth = GetDeviceCaps(screenDC, HORZRES);
        giMainVideoModeHeight = GetDeviceCaps(screenDC, VERTRES);
        ReleaseDC(NULL, screenDC);
        if (giMainVideoModeColorDepth < WINGRAPH_COLOR_DEPTH)
            ShutDown(
                "Heroes II requires 256 color mode or higher.\n\n"
                "To change color mode, right click in an open area on the Windows 95 "
                "background, choose 'Properties', then the 'Settings' tab, then change "
                "the entry in the 'Color Palette Box'."
            );
    }
}

VA(0x00437308, 0xa0)
void InitGraphics(void) {
    LogStr("IG1");
    ConnectToDLLs();
    LogStr("IG2");
    if (gConfig.gfx[giCurExe].fullScreen != 0)
        giGraphicsType = WINGRAPH_GRAPHICS_DIRECT_DRAW;
    else
        giGraphicsType = WINGRAPH_GRAPHICS_WING;
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING) {
        LogStr("IG3");
        WGInitGraphics();
        LogStr("IG4");
    } else {
        LogStr("IG5");
        DDInitGraphics();
        LogStr("IG6");
    }
}

VA(0x004373a8, 0x46)
i32 AppPaint(void* window, void* message) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        return WGAppPaint(window, message);
    else
        return DDAppPaint(window, message);
}

VA(0x004373ee, 0x2c)
void InitializePalette(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        WGInitializePalette();
    else
        DDInitializePalette();
}

VA(0x0043741a, 0x38)
void UpdatePalette(i8* paletteData) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        WGUpdatePalette(paletteData);
    else
        DDUpdatePalette(paletteData);
}

VA(0x00437452, 0x31)
void CleanUpWinGraphics(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        WGCleanUpWinGraphics();
    else
        DDCleanUpWinGraphics();
    DisconnectDLLs();
}

VA(0x00437483, 0xe1)
void SetFullScreenStatus(i32 fullScreen) {
    if (gbInSmackMgr != 0)
        return;
    if (gConfig.gfx[giCurExe].fullScreen == fullScreen)
        return;
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING) {
        if (gbDDrawAttached == 0)
            return;
        gConfig.gfx[giCurExe].fullScreen = 1;
        if (SetGraphicsType(WINGRAPH_GRAPHICS_DIRECT_DRAW) != 0)
            DDSetFullScreenStatus(fullScreen);
    } else if (fullScreen == 0) {
        if (gbWinGAttached != 0)
            SetGraphicsType(WINGRAPH_GRAPHICS_WING);
    } else {
        DDSetFullScreenStatus(fullScreen);
    }
    if (fullScreen != 0)
        CheckChangeCursor(0, 0, 1);
}

VA(0x00437564, 0x31)
i32 QueryNewPalette(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        return WGQueryNewPalette();
    else
        return DDQueryNewPalette();
}

VA(0x00437595, 0x286)
i32 SetGraphicsType(WingraphGraphicsType graphicsType) {
    i32 fullScreen;
    i32 x;
    i32 y;
    i32 width;
    i32 height7;
    void* screenBuffer;

    if (OD_STEER(giGraphicsType) == graphicsType)
        return 1;
    if (graphicsType == WINGRAPH_GRAPHICS_WING && gbWinGAttached == 0)
        return 0;
    if (graphicsType == WINGRAPH_GRAPHICS_DIRECT_DRAW && gbDDrawAttached == 0)
        return 0;

    fullScreen = gConfig.gfx[giCurExe].fullScreen;
    x = gConfig.gfx[giCurExe].x;
    y = gConfig.gfx[giCurExe].y;
    width = gConfig.gfx[giCurExe].width;
    height7 = gConfig.gfx[giCurExe].height;
    screenBuffer = H2_ALLOC(WINGRAPH_WIDTH * WINGRAPH_HEIGHT, 1265);
    memcpy(screenBuffer, gpWindowManager->m_screen->m_pixels, WINGRAPH_WIDTH * WINGRAPH_HEIGHT);
    if (graphicsType == WINGRAPH_GRAPHICS_WING) {
        gConfig.gfx[giCurExe].fullScreen = 0;
        DDCleanUpWinGraphics();
        giGraphicsType = WINGRAPH_GRAPHICS_WING;
        WGInitGraphics();
        gpWindowManager->m_screen->m_pixels = static_cast<u8*>(lpInitWin);
    } else {
        WGCleanUpWinGraphics();
        giGraphicsType = WINGRAPH_GRAPHICS_DIRECT_DRAW;
        DDInitGraphics();
        gpWindowManager->m_screen->m_pixels = static_cast<u8*>(lpInitWin);
    }
    memcpy(gpWindowManager->m_screen->m_pixels, screenBuffer, WINGRAPH_WIDTH * WINGRAPH_HEIGHT);
    H2_FREE(screenBuffer, 1286);
    if (fullScreen != 0 && graphicsType == WINGRAPH_GRAPHICS_WING) {
        SetMenuStatus(1);
        ResizeWindow(x, y, width, height7);
    }
    BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0, WINGRAPH_WIDTH, WINGRAPH_HEIGHT, 0, 0);
    UpdatePalette(gpBufferPalette->m_data);
    return 1;
}

DATA(0x004f1288) b32 gbWinGAttached = true;
DATA(0x004f128c) b32 gbDDrawAttached = false;
DATA(0x004f1290) WingraphGraphicsType giGraphicsType = WINGRAPH_GRAPHICS_WING;
DATA(0x004f1294) i32l Orientation = 1;
DATA(0x004f1298) struct _PALETTE LogicalPalette = {WINGRAPH_PALETTE_VERSION, WINGRAPH_PALETTE_SIZE};
DATA(0x004f169c) void* lpInitWin = NULL;
DATA(0x004f16a0) i32 bPaletteInitialized = 0;
DATA(0x004f16a4) i32 giTtlBlts = 0;
DATA(0x004f16a8) b32 gbWinGraphBusy = false;
DATA(0x004f16ac) DirectDrawCreateProc lpDirectDrawCreate = NULL;
DATA(0x004f16b0) struct IDirectDraw* lpDD = NULL;
DATA(0x004f16b4) struct IDirectDrawSurface* lpDDSPrimary = NULL;
DATA(0x004f16b8) struct IDirectDrawSurface* lpDDSOne = NULL;
DATA(0x004f16bc) struct IDirectDrawClipper* lpClipper = NULL;
DATA(0x004f16c0) struct IDirectDrawPalette* lpDDPal = NULL;
DATA(0x004f17b0) i32 iBusyRetry = 0;
DATA(0x004f1a14) i32 bInDDSD = 0;
DATA(0x004f1e78) HDC hdcImage = NULL;
DATA(0x004f1e7c) HBITMAP gbmOldMonoBitmap = NULL;
DATA(0x004f1e80) HPALETTE hpalApp = NULL;
DATA(0x004f1f60) HINSTANCE hWinGLibrary = NULL;
DATA(0x004f1f64) HINSTANCE hDDrawLibrary = NULL;
DATA(0x004f2080) i32l lDelayRefresh = 0;
DATA(0x00525168) i32l lPaintStart;
DATA(0x00525180) struct _IMAGE screenImage;

#undef RETAIL_FILE
