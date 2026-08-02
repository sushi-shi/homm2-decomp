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

#define RETAIL_FILE "e:\\Users\\igorl\\VSS\\HMM\\HMM2\\Source\\Game\\WINGRAPH.CPP"

H2_ENUM_BEGIN(WingraphPaletteConstant)
    PALETTE_COMPONENT_COUNT = 3,
    PALETTE_RED_COMPONENT = 0,
    PALETTE_GREEN_COMPONENT = 1,
    PALETTE_BLUE_COMPONENT = 2,
    PALETTE_VALUE_SHIFT = 2,
    SYSTEM_PALETTE_REGION_COUNT = 2
H2_ENUM_END(WingraphPaletteConstant)

static RECT gDDSourceRect;
static RECT gDDClientRect;
static RECT gDDDestinationRect;
static HRESULT gDDResult;
static DDSURFACEDESC gDDSurfaceDesc;

VA(0x004afbe0, 0x3a)
void DDRestoreDisplayMode(void) {
    HRESULT result;

    if (lpDD != NULL) {
        result = lpDD->RestoreDisplayMode();
        if (result != DD_OK)
            DDSD(result, RETAIL_FILE, 52);
    }
}

VA(0x004afc1a, 0x2d)
i32 DDQueryNewPalette(void) {
    i32 unused;

    if (gbWinGraphBusy != 0)
        return 1;
    if (gbForegroundApp == 0)
        return 1;
    return SetPalette();
}

VA(0x004afc47, 0x76)
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

VA(0x004afcbd, 0xb2)
void SetupClipper(void) {
    HRESULT result;

    if (gConfig.gfx[IDX(giCurExe)].fullScreen == 0) {
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

VA(0x004afd6f, 0x120)
void DDInitGraphics(void) {
    HRESULT result;

    if (gbWinGraphBusy != 0)
        return;
    result = lpDirectDrawCreate(NULL, &lpDD, NULL);
    if (result != DD_OK)
        DDSD(result, RETAIL_FILE, 118);
    if (gConfig.gfx[IDX(giCurExe)].fullScreen != 0) {
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

VA(0x004afe8f, 0x4da)
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
        gDDSourceRect.right = sourceWidth5 + sourceLeft3 - 1;
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

VA(0x004b0369, 0x111)
void DDInitializePalette(void) {
    HDC winDC;
    i32 entry0;
    HRESULT rr;

    if (gbWinGraphBusy != 0)
        return;
    {
        winDC = GetDC(NULL);
        GetSystemPaletteEntries(winDC, 0, WINGRAPH_SYSTEM_PALETTE_SIZE, LogicalPalette.entries);
        GetSystemPaletteEntries(
            winDC,
            WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE,
            WINGRAPH_SYSTEM_PALETTE_SIZE,
            &LogicalPalette.entries[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE]
        );
        ReleaseDC(NULL, winDC);
        for (entry0 = 0; entry0 < WINGRAPH_SYSTEM_PALETTE_SIZE; entry0++) {
            LogicalPalette.entries[entry0].peFlags = 0;
            LogicalPalette.entries[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry0]
                .peFlags = 0;
        }
        for (entry0 = WINGRAPH_SYSTEM_PALETTE_SIZE;
             entry0 < WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE;
             entry0++) {
            LogicalPalette.entries[entry0].peRed = 0;
            LogicalPalette.entries[entry0].peGreen = 0;
            LogicalPalette.entries[entry0].peBlue = 0;
            LogicalPalette.entries[entry0].peFlags = PC_NOCOLLAPSE;
        }
        rr = lpDD->CreatePalette(DDPCAPS_8BIT, LogicalPalette.entries, &lpDDPal, NULL);
        if (rr != DD_OK)
            DDSD(rr, RETAIL_FILE, 359);
        SetPalette();
    }
}

VA(0x004b047a, 0x7e)
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

VA(0x004b04f8, 0x109)
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

VA(0x004b0601, 0x57f)
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

VA(0x004b0c15, 0xee)
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
        LogicalPalette.entries[entry].peRed =
            paletteData[entry * PALETTE_COMPONENT_COUNT + PALETTE_RED_COMPONENT]
            << PALETTE_VALUE_SHIFT;
        LogicalPalette.entries[entry].peGreen =
            paletteData[entry * PALETTE_COMPONENT_COUNT + PALETTE_GREEN_COMPONENT]
            << PALETTE_VALUE_SHIFT;
        LogicalPalette.entries[entry].peBlue =
            paletteData[entry * PALETTE_COMPONENT_COUNT + PALETTE_BLUE_COMPONENT]
            << PALETTE_VALUE_SHIFT;
        LogicalPalette.entries[entry].peFlags = PC_NOCOLLAPSE;
    }
    ProcessAssert(reinterpret_cast<i32>(lpDDPal), RETAIL_FILE, 518);
    result0 = lpDDPal->SetEntries(
        0,
        WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE * IDX(SYSTEM_PALETTE_REGION_COUNT),
        &LogicalPalette.entries[WINGRAPH_SYSTEM_PALETTE_SIZE]
    );
    if (result0 != DD_OK)
        DDSD(result0, RETAIL_FILE, 522);
}

VA(0x004b0d03, 0x14a)
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

VA(0x004b0e4d, 0x279)
void DDSetFullScreenStatus(i32 fullScreen) {
    i32 width;
    i32 windowHeight0;
    i32 x;
    i32 y;
    HRESULT result0;

    if (gbWinGraphBusy != 0)
        return;
    if (gConfig.gfx[IDX(giCurExe)].fullScreen == fullScreen)
        return;
    {
        x = gConfig.gfx[IDX(giCurExe)].x;
        y = gConfig.gfx[IDX(giCurExe)].y;
        width = gConfig.gfx[IDX(giCurExe)].width;
        windowHeight0 = gConfig.gfx[IDX(giCurExe)].height;
        gbWinGraphBusy = true;
        gConfig.gfx[IDX(giCurExe)].fullScreen = fullScreen;
        if (gConfig.gfx[IDX(giCurExe)].fullScreen != 0)
            SetMenuStatus(0);

        result0 = lpDD->SetCooperativeLevel(
            hwndApp,
            DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT
        );
        if (result0 != DD_OK)
            DDSD(result0, RETAIL_FILE, 593);
        if (gConfig.gfx[IDX(giCurExe)].fullScreen != 0) {
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
        if (gConfig.gfx[IDX(giCurExe)].fullScreen == 0) {
            SetMenuStatus(1);
            ResizeWindow(x, y, width, windowHeight0);
        } else {
            gConfig.gfx[IDX(giCurExe)].x = x;
            gConfig.gfx[IDX(giCurExe)].y = y;
            gConfig.gfx[IDX(giCurExe)].width = width;
            gConfig.gfx[IDX(giCurExe)].height = windowHeight0;
        }
        SetupClipper();
    }
}

VA(0x004b10c6, 0x72)
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

VA(0x004b1138, 0x13b)
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
    screenImage.header.biSizeImage = screenImage.header.biWidth * screenImage.header.biHeight;
    screenImage.header.biSizeImage *= Orientation;
    gbmOldMonoBitmap = static_cast<HBITMAP>(SelectObject(hdcImage, bitmap));
    lpInitWin = screenImage.bits;
    PatBlt(hdcImage, 0, 0, iMainWinScreenWidth, iMainWinScreenHeight, BLACKNESS);
}

VA(0x004b1273, 0x1c1)
void WGUpdatePalette(i8* paletteData) {
    HDC dc0;
    i32 entry;
    i32 result0;

    for (entry = WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry < WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry++) {
        LogicalPalette.entries[entry].peRed =
            paletteData[entry * PALETTE_COMPONENT_COUNT + PALETTE_RED_COMPONENT]
            << PALETTE_VALUE_SHIFT;
        screenImage.colors[entry].rgbRed = LogicalPalette.entries[entry].peRed;
        LogicalPalette.entries[entry].peGreen =
            paletteData[entry * PALETTE_COMPONENT_COUNT + PALETTE_GREEN_COMPONENT]
            << PALETTE_VALUE_SHIFT;
        screenImage.colors[entry].rgbGreen = LogicalPalette.entries[entry].peGreen;
        LogicalPalette.entries[entry].peBlue =
            paletteData[entry * PALETTE_COMPONENT_COUNT + PALETTE_BLUE_COMPONENT]
            << PALETTE_VALUE_SHIFT;
        screenImage.colors[entry].rgbBlue = LogicalPalette.entries[entry].peBlue;
    }
    AnimatePalette(
        hpalApp,
        WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE * IDX(SYSTEM_PALETTE_REGION_COUNT),
        &LogicalPalette.entries[WINGRAPH_SYSTEM_PALETTE_SIZE]
    );
    WinGSetDIBColorTable(
        hdcImage,
        WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE * IDX(SYSTEM_PALETTE_REGION_COUNT),
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

VA(0x004b1434, 0x1a2)
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

VA(0x004b15d6, 0x1bd)
i32 WGAppPaint(void* window, void* paintDC) {
    i32 padding8;
    char unusedByte0;
    RECT clientRect16;
    i32 destY0;
    i32 xSource;
    PAINTSTRUCT paint5;
    i32 destHeight3;
    i32 destW;
    i32 destX7;
    i32 fromY;

    unusedByte0 = 0;
    if (screenImage.bits != NULL) {
        paintDC = BeginPaint(reinterpret_cast<HWND>(window), &paint5);
        SelectPalette(static_cast<HDC>(paintDC), hpalApp, 0);
        RealizePalette(static_cast<HDC>(paintDC));
        GetClientRect(reinterpret_cast<HWND>(window), &clientRect16);
        destX7 = 0;
        xSource = destX7;
        destY0 = 0;
        fromY = destY0;
        destW = clientRect16.right - clientRect16.left;
        destHeight3 = clientRect16.bottom - clientRect16.top;
        xSource = (destX7 * WINGRAPH_WIDTH) / iMainWinScreenWidth;
        fromY = (destY0 * WINGRAPH_HEIGHT) / iMainWinScreenHeight;
        if (giScrollX != 0)
            xSource += giScrollX;
        if (giScrollY != 0)
            fromY += giScrollY;
        giTtlBlts++;
        if (iMainWinScreenWidth == WINGRAPH_WIDTH && iMainWinScreenHeight == WINGRAPH_HEIGHT) {
            destX7 = paint5.rcPaint.left & WINGRAPH_PAINT_ALIGN_MASK;
            destW = paint5.rcPaint.right - destX7 + 1;
            destY0 = paint5.rcPaint.top;
            destHeight3 = paint5.rcPaint.bottom - destY0 + 1;
            WinGBitBlt(
                static_cast<HDC>(paintDC),
                destX7,
                destY0,
                destW,
                destHeight3,
                hdcImage,
                destX7 + giScrollX,
                destY0 + giScrollY
            );
        } else {
            WinGStretchBlt(
                static_cast<HDC>(paintDC),
                destX7,
                destY0,
                destW,
                destHeight3,
                hdcImage,
                xSource,
                fromY,
                (destW * WINGRAPH_WIDTH) / iMainWinScreenWidth,
                (destHeight3 * WINGRAPH_HEIGHT) / iMainWinScreenHeight
            );
        }
        EndPaint(reinterpret_cast<HWND>(window), &paint5);
    }
    return 1;
}

VA(0x004b1793, 0x67)
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

VA(0x004b17fa, 0x66)
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
        gConfig.gfx[IDX(giCurExe)].fullScreen = 0;
        SetMenuStatus(1);
    }
}

VA(0x004b1860, 0x1a)
void DisconnectDLLs(void) {
    if (reinterpret_cast<u32l>(hDDrawLibrary) >= WINGRAPH_LOAD_LIBRARY_SUCCESS) {
        FreeLibrary(hDDrawLibrary);
    }
}

VA(0x004b187a, 0x15)
void RestoreDisplayMode(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING) {
        return;
    } else {
        DDRestoreDisplayMode();
    }
}

VA(0x004b188f, 0x17)
i32 SetPalette(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        return 0;
    else
        return DDSetPalette();
}

VA(0x004b18a6, 0x6b)
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
                "\xc4\xeb\xff \xc3\xe5\xf0\xee\xe5\xe2 II \xf2\xf0\xe5\xe1\xf3\xe5\xf2\xf1\xff "
                "\xf0\xe5\xe6\xe8\xec \xe2 256 \xf6\xe2\xe5\xf2\xee\xe2 \xe8\xeb\xe8 "
                "\xe1\xee\xeb\xfc\xf8\xe5.\n\n\xd7\xf2\xee\xe1\xfb "
                "\xe8\xe7\xec\xe5\xed\xe8\xf2\xfc \xf0\xe5\xe6\xe8\xec "
                "\xf6\xe2\xe5\xf2\xed\xee\xf1\xf2\xe8, \xf9\xe5\xeb\xea\xed\xe8\xf2\xe5 "
                "\xef\xf0\xe0\xe2\xee\xe9 \xea\xed\xee\xef\xea\xee\xe9 \xef\xee "
                "\xf0\xe0\xe1\xee\xf7\xe5\xec\xf3 \xf1\xf2\xee\xeb\xf3 Windows \xe8 "
                "\xe2\xfb\xe1\xe5\xf0\xe8\xf2\xe5 'Properties'/\xd1\xe2\xee\xe9\xf1\xf2\xe2\xe0'. "
                "\xc7\xe0\xf2\xe5\xec \xe2 \xf3\xf1\xf2\xe0\xed\xee\xe2\xea\xe0\xf5 "
                "\xe2\xfb\xe1\xe5\xf0\xe8\xf2\xe5 \xe3\xeb\xf3\xe1\xe8\xed\xf3 "
                "\xf6\xe2\xe5\xf2\xe0." /* "Для Героев II требуется режим в 256 цветов или больше.

Чтобы изменить режим цветности, щелкните правой кнопкой по рабочему столу Windows и выберите 'Properties'/Свойства'. Затем в установках выберите глубину цвета." */
            );
    }
}

VA(0x004b1911, 0x82)
void InitGraphics(void) {
    LogStr("IG1");
    ConnectToDLLs();
    LogStr("IG2");
    if (gConfig.gfx[IDX(giCurExe)].fullScreen != 0)
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

VA(0x004b1993, 0x31)
i32 AppPaint(void* window, void* message) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        return WGAppPaint(window, message);
    else
        return DDAppPaint(window, message);
}

VA(0x004b19c4, 0x1a)
void InitializePalette(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        WGInitializePalette();
    else
        DDInitializePalette();
}

VA(0x004b19de, 0x28)
void UpdatePalette(i8* paletteData) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        WGUpdatePalette(paletteData);
    else
        DDUpdatePalette(paletteData);
}

VA(0x004b1a06, 0x1f)
void CleanUpWinGraphics(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        WGCleanUpWinGraphics();
    else
        DDCleanUpWinGraphics();
    DisconnectDLLs();
}

VA(0x004b1a25, 0xa1)
void SetFullScreenStatus(i32 fullScreen) {
    if (gbInSmackMgr != 0)
        return;
    if (gConfig.gfx[IDX(giCurExe)].fullScreen == fullScreen)
        return;
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING) {
        if (gbDDrawAttached == 0)
            return;
        gConfig.gfx[IDX(giCurExe)].fullScreen = 1;
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

VA(0x004b1ac6, 0x1a)
i32 QueryNewPalette(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        return WGQueryNewPalette();
    else
        return DDQueryNewPalette();
}

VA(0x004b1ae0, 0x1ce)
i32 SetGraphicsType(WingraphGraphicsType graphicsType) {
    i32 fullScreen;
    i32 x;
    i32 y;
    i32 width;
    i32 height7;
    void* screenBuffer;

    if (giGraphicsType == graphicsType)
        return 1;
    if (graphicsType == WINGRAPH_GRAPHICS_WING && gbWinGAttached == 0)
        return 0;
    if (graphicsType == WINGRAPH_GRAPHICS_DIRECT_DRAW && gbDDrawAttached == 0)
        return 0;

    fullScreen = gConfig.gfx[IDX(giCurExe)].fullScreen;
    x = gConfig.gfx[IDX(giCurExe)].x;
    y = gConfig.gfx[IDX(giCurExe)].y;
    width = gConfig.gfx[IDX(giCurExe)].width;
    height7 = gConfig.gfx[IDX(giCurExe)].height;
    screenBuffer = H2_ALLOC(WINGRAPH_WIDTH * WINGRAPH_HEIGHT);
    memcpy(screenBuffer, gpWindowManager->m_screen->m_pixels, WINGRAPH_WIDTH * WINGRAPH_HEIGHT);
    if (graphicsType == WINGRAPH_GRAPHICS_WING) {
        gConfig.gfx[IDX(giCurExe)].fullScreen = 0;
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
    H2_FREE(screenBuffer);
    if (fullScreen != 0 && graphicsType == WINGRAPH_GRAPHICS_WING) {
        SetMenuStatus(1);
        ResizeWindow(x, y, width, height7);
    }
    BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0, WINGRAPH_WIDTH, WINGRAPH_HEIGHT, 0, 0);
    UpdatePalette(gpBufferPalette->m_data);
    return 1;
}

b32 gbWinGAttached = true;
b32 gbDDrawAttached = false;
WingraphGraphicsType giGraphicsType = WINGRAPH_GRAPHICS_WING;
i32l Orientation = 1;
struct _PALETTE LogicalPalette = {WINGRAPH_PALETTE_VERSION, WINGRAPH_PALETTE_SIZE};
void* lpInitWin = NULL;
i32 bPaletteInitialized = 0;
i32 giTtlBlts = 0;
b32 gbWinGraphBusy = false;
DirectDrawCreateProc lpDirectDrawCreate = NULL;
struct IDirectDraw* lpDD = NULL;
struct IDirectDrawSurface* lpDDSPrimary = NULL;
struct IDirectDrawSurface* lpDDSOne = NULL;
struct IDirectDrawClipper* lpClipper = NULL;
struct IDirectDrawPalette* lpDDPal = NULL;
i32 iBusyRetry = 0;
i32 bInDDSD = 0;
HDC hdcImage = NULL;
HBITMAP gbmOldMonoBitmap = NULL;
HPALETTE hpalApp = NULL;
HINSTANCE hWinGLibrary = NULL;
HINSTANCE hDDrawLibrary = NULL;
i32l lDelayRefresh = 0;
i32l lPaintStart;
struct _IMAGE screenImage;
