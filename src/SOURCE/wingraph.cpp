#include <stdio.h>
#include <string.h>
#include <Ints.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <BASE/palette.h>
#include <BASE/INPUTMGR.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/wingraph.h>


typedef enum WingraphPaletteConstant {
    PALETTE_COMPONENT_COUNT = 3,
    PALETTE_RED_COMPONENT = 0,
    PALETTE_GREEN_COMPONENT = 1,
    PALETTE_BLUE_COMPONENT = 2,
    PALETTE_VALUE_SHIFT = 2,
    SYSTEM_PALETTE_REGION_COUNT = 2
} WingraphPaletteConstant;

static RECT gDDClientRect = {0};
static HRESULT gDDResult = 0;
static DDSURFACEDESC gDDSurfaceDesc = {0};
struct _IMAGE screenImage = {0};
static RECT gDDSourceRect = {0};
i32l lPaintStart = 0;
static RECT gDDDestinationRect = {0};

void DDRestoreDisplayMode(void) {
    HRESULT result;

    if (lpDD != NULL) {
        result = lpDD->RestoreDisplayMode();
        if (result != DD_OK)
            DDSD(
                result,
                "wingraph.cpp",
                52
            );
    }
}

i32 DDQueryNewPalette(void) {
    i32 unused;

    if (gbWinGraphBusy != 0)
        return 1;
    if (gbForegroundApp == 0)
        return 1;
    return SetPalette();
}

void CreatePrimary(void) {
    HRESULT result;

    lpDDSPrimary = DDCreateSurface(WINGRAPH_WIDTH, WINGRAPH_HEIGHT, 1);
    if (lpClipper != NULL) {
        result = lpDDSPrimary->SetClipper(NULL);
        if (result != DD_OK && result != DDERR_NOCLIPPERATTACHED)
            DDSD(result, "wingraph.cpp", 77);
        lpClipper->Release();
        lpClipper = NULL;
    }
}

void SetupClipper(void) {
    HRESULT result;

    if (gConfig.gfx[(giCurExe)].fullScreen == 0) {
        result = lpDD->CreateClipper(0, &lpClipper, NULL);
        if (result != DD_OK)
            DDSD(
                result,
                "wingraph.cpp",
                95
            );
        result = lpClipper->SetHWnd(0, hwndApp);
        if (result != DD_OK)
            DDSD(
                result,
                "wingraph.cpp",
                100
            );
        result = lpDDSPrimary->SetClipper(lpClipper);
        if (result != DD_OK)
            DDSD(
                result,
                "wingraph.cpp",
                105
            );
    }
}

void DDInitGraphics(void) {
    HRESULT result;

    if (gbWinGraphBusy != 0)
        return;
    result = lpDirectDrawCreate(NULL, &lpDD, NULL);
    if (result != DD_OK)
        DDSD(
            result,
            "wingraph.cpp",
            118
        );
    if (gConfig.gfx[(giCurExe)].fullScreen != 0) {
        SetMenuStatus(0);
        result = lpDD->SetCooperativeLevel(
            hwndApp,
            DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT
        );
        if (result != DD_OK)
            DDSD(
                result,
                "wingraph.cpp",
                130
            );
        result = lpDD->SetDisplayMode(WINGRAPH_WIDTH, WINGRAPH_HEIGHT, WINGRAPH_COLOR_DEPTH);
        if (result != DD_OK)
            DDSD(
                result,
                "wingraph.cpp",
                134
            );
    } else {
        result = lpDD->SetCooperativeLevel(hwndApp, DDSCL_NORMAL);
        if (result != DD_OK)
            DDSD(
                result,
                "wingraph.cpp",
                141
            );
    }
    CreatePrimary();
    SetupClipper();
    lpDDSOne = DDCreateSurface(WINGRAPH_WIDTH, WINGRAPH_HEIGHT, 0);
    InitializePalette();
}

i32 DDAppPaint(void* window, void* paintDC) {
    i32 sourceWidth;
    i32 sourceHeight5;
    i32 sourceTop8;
    i32 sourceLeft6;
    PAINTSTRUCT paint7;
    POINT point1;

    if (gbWinGraphBusy != 0)
        return 1;
    if (gbMinimized != 0)
        return 1;
    if (lpDD == NULL)
        return 1;
    {
        gbWinGraphBusy = true;
        paintDC = BeginPaint(reinterpret_cast<HWND>(window), &paint7);
        GetClientRect(reinterpret_cast<HWND>(window), &gDDClientRect);
        if (paint7.rcPaint.right == 0 || paint7.rcPaint.bottom == 0)
            paint7.rcPaint = gDDClientRect;
        if (paint7.rcPaint.right < WINGRAPH_WIDTH)
            paint7.rcPaint.right++;
        if (paint7.rcPaint.bottom < WINGRAPH_HEIGHT)
            paint7.rcPaint.bottom++;

        gDDDestinationRect = paint7.rcPaint;
        sourceWidth = ((gDDDestinationRect.right - gDDDestinationRect.left + 1) * WINGRAPH_WIDTH)
                       / iMainWinScreenWidth;
        sourceHeight5 = ((gDDDestinationRect.bottom - gDDDestinationRect.top + 1) * WINGRAPH_HEIGHT)
                        / iMainWinScreenHeight;
        sourceLeft6 = (gDDDestinationRect.left * WINGRAPH_WIDTH) / iMainWinScreenWidth;
        sourceTop8 = (gDDDestinationRect.top * WINGRAPH_HEIGHT) / iMainWinScreenHeight;
        if (giScrollX != 0) {
            sourceLeft6 = giScrollX + WINGRAPH_SCROLL_MARGIN;
            sourceWidth = WINGRAPH_SCROLL_SIZE;
        }
        if (giScrollY != 0) {
            sourceTop8 = giScrollY + WINGRAPH_SCROLL_MARGIN;
            sourceHeight5 = WINGRAPH_SCROLL_SIZE;
        }
        gDDSourceRect.left = sourceLeft6;
        gDDSourceRect.right = sourceLeft6 + sourceWidth - 1;
        gDDSourceRect.top = sourceTop8;
        gDDSourceRect.bottom = sourceTop8 + sourceHeight5 - 1;

        point1.y = 0;
        point1.x = point1.y;
        ClientToScreen(hwndApp, &point1);
        OffsetRect(&gDDDestinationRect, point1.x, point1.y);
        gDDResult = lpDDSOne->Unlock(NULL);
        if (gDDResult != DD_OK)
            DDSD(
                gDDResult,
                "wingraph.cpp",
                228
            );

        if (gDDSourceRect.left < 0)
            gDDSourceRect.left = 0;
        if (gDDSourceRect.top < 0)
            gDDSourceRect.top = 0;
        if (gDDSourceRect.right > WINGRAPH_WIDTH)
            gDDSourceRect.right = WINGRAPH_WIDTH;
        if (gDDSourceRect.bottom > WINGRAPH_HEIGHT)
            gDDSourceRect.bottom = WINGRAPH_HEIGHT;

        lPaintStart = KBTickCount();
        while (1) {
            gDDResult =
                lpDDSPrimary->Blt(&gDDDestinationRect, lpDDSOne, &gDDSourceRect, DDBLT_WAIT, NULL);
            if (gDDResult == DDERR_SURFACELOST) {
                gDDResult = lpDDSPrimary->Restore();
                if (gDDResult == DDERR_WRONGMODE) {
                    LogStr("ResetDisplayMode");
                    gDDResult =
                        lpDD->SetDisplayMode(WINGRAPH_WIDTH, WINGRAPH_HEIGHT, WINGRAPH_COLOR_DEPTH);
                    if (gDDResult != DD_OK)
                        DDSD(
                            gDDResult,
                            "wingraph.cpp",
                            248
                        );
                    gDDResult = lpDDSPrimary->Restore();
                    if (gDDResult != DD_OK)
                        DDSD(
                            gDDResult,
                            "wingraph.cpp",
                            252
                        );
                    gDDDestinationRect = gDDSourceRect;
                }
                if (gDDResult != DD_OK)
                    DDSD(
                        gDDResult,
                        "wingraph.cpp",
                        260
                    );
            } else if (gDDResult == DDERR_SURFACEBUSY
                       && KBTickCount() < lPaintStart + WINGRAPH_PAINT_TIMEOUT) {
                iBusyRetry++;
            } else if (gDDResult != DD_OK) {
                DDSD(
                    gDDResult,
                    "wingraph.cpp",
                    265
                );
            } else {
                break;
            }
        }

        memset(&gDDSurfaceDesc, 0, sizeof(gDDSurfaceDesc));
        gDDSurfaceDesc.dwSize = sizeof(gDDSurfaceDesc);
        gDDResult = lpDDSOne->Lock(NULL, &gDDSurfaceDesc, DDLOCK_WAIT, NULL);
        if (gDDResult != DD_OK)
            DDSD(
                gDDResult,
                "wingraph.cpp",
                275
            );
        if (gpWindowManager->m_screen != NULL) {
            gpWindowManager->m_screen->m_pixels = static_cast<u8*>(gDDSurfaceDesc.lpSurface);
            lpInitWin = gDDSurfaceDesc.lpSurface;
        } else {
            lpInitWin = gDDSurfaceDesc.lpSurface;
        }
        if (gDDResult != DD_OK)
            DDSD(
                gDDResult,
                "wingraph.cpp",
                286
            );
        EndPaint(reinterpret_cast<HWND>(window), &paint7);
        gbWinGraphBusy = false;
    }
    return 1;
}

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
            DDSD(
                rr,
                "wingraph.cpp",
                359
            );
        SetPalette();
    }
}

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
        DDSD(
            result,
            "wingraph.cpp",
            383
        );
    return 0;
}

struct IDirectDrawSurface* DDCreateSurface(u32l width, u32l height, i32 primary) {
    DDSURFACEDESC ddsd;
    IDirectDrawSurface* lpSurface;
    i32 cnt;
    i32 unused;
    HRESULT rv;

    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    if (primary != 0) {
        ddsd.dwFlags = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    } else {
        ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
        ddsd.dwHeight = height;
        ddsd.dwWidth = width;
    }
    rv = lpDD->CreateSurface(&ddsd, &lpSurface, NULL);
    if (rv != DD_OK)
        DDSD(
            rv,
            "wingraph.cpp",
            421
        );
    if (primary == 0) {
        rv = lpSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
        if (rv != DD_OK)
            DDSD(
                rv,
                "wingraph.cpp",
                429
            );
        if (gpWindowManager->m_screen != NULL) {
            gpWindowManager->m_screen->m_pixels = static_cast<u8*>(ddsd.lpSurface);
            lpInitWin = ddsd.lpSurface;
        } else {
            lpInitWin = ddsd.lpSurface;
        }
    }
    return lpSurface;
}

void DDSD(i32 error, char* file, i32 line) {
    HRESULT hres;
    i32 unused;
    char errorMessage[WINGRAPH_ERROR_TEXT_SIZE];

    if (bInDDSD != 0)
        return;
    bInDDSD = 1;
    hres = lpDD->RestoreDisplayMode();
    unused = 0;
    switch (error) {
        case DD_OK:
            return;
        case DDERR_GENERIC:
            strcpy(errorMessage, "DDERR_GENERIC            ");
            break;
        case DDERR_INVALIDCLIPLIST:
            strcpy(errorMessage, "DDERR_INVALIDCLIPLIST\t   ");
            break;
        case DDERR_INVALIDOBJECT:
            strcpy(errorMessage, "DDERR_INVALIDOBJECT      ");
            break;
        case DDERR_INVALIDPARAMS:
            strcpy(errorMessage, "DDERR_INVALIDPARAMS\t   ");
            break;
        case DDERR_INVALIDRECT:
            strcpy(errorMessage, "DDERR_INVALIDRECT\t      ");
            break;
        case DDERR_NOALPHAHW:
            strcpy(errorMessage, "DDERR_NOALPHAHW          ");
            break;
        case DDERR_NOBLTHW:
            strcpy(errorMessage, "DDERR_NOBLTHW\t         ");
            break;
        case DDERR_NOCLIPLIST:
            strcpy(errorMessage, "DDERR_NOCLIPLIST         ");
            break;
        case DDERR_NODDROPSHW:
            strcpy(errorMessage, "DDERR_NODDROPSHW\t      ");
            break;
        case DDERR_SURFACELOST:
            strcpy(errorMessage, "DDERR_SURFACELOST        ");
            break;
        case DDERR_UNSUPPORTED:
            strcpy(errorMessage, "DDERR_UNSUPPORTED\t      ");
            break;
        case DDERR_NOMIRRORHW:
            strcpy(errorMessage, "DDERR_NOMIRRORHW         ");
            break;
        case DDERR_NORASTEROPHW:
            strcpy(errorMessage, "DDERR_NORASTEROPHW\t      ");
            break;
        case DDERR_NOROTATIONHW:
            strcpy(errorMessage, "DDERR_NOROTATIONHW       ");
            break;
        case DDERR_NOSTRETCHHW:
            strcpy(errorMessage, "DDERR_NOSTRETCHHW\t      ");
            break;
        case DDERR_SURFACEBUSY:
            strcpy(errorMessage, "DDERR_SURFACEBUSY        ");
            break;
        case DDERR_NOZBUFFERHW:
            strcpy(errorMessage, "DDERR_NOZBUFFERHW        ");
            break;
        case DDERR_OUTOFMEMORY:
            strcpy(errorMessage, "DDERR_OUTOFMEMORY\t      ");
            break;
        case DDERR_CLIPPERISUSINGHWND:
            strcpy(errorMessage, "DDERR_CLIPPERISUSINGHWND ");
            break;
        case DDERR_NOEXCLUSIVEMODE:
            strcpy(errorMessage, "DDERR_NOEXCLUSIVEMODE\t   ");
            break;
        case DDERR_NOT8BITCOLOR:
            strcpy(errorMessage, "DDERR_NOT8BITCOLOR       ");
            break;
        case DDERR_NOPALETTEATTACHED:
            strcpy(errorMessage, "DDERR_NOPALETTEATTACHED  ");
            break;
        case DDERR_NOPALETTEHW:
            strcpy(errorMessage, "DDERR_NOPALETTEHW\t      ");
            break;
        case DDERR_LOCKEDSURFACES:
            strcpy(errorMessage, "DDERR_LOCKEDSURFACES     ");
            break;
        case DDERR_IMPLICITLYCREATED:
            strcpy(errorMessage, "DDERR_IMPLICITLYCREATED  ");
            break;
        case DDERR_WRONGMODE:
            strcpy(errorMessage, "DDERR_WRONGMODE          ");
            break;
        case DDERR_INCOMPATIBLEPRIMARY:
            strcpy(errorMessage, "DDERR_INCOMPATIBLEPRIMARY");
            break;
        case DDERR_NOCLIPPERATTACHED:
            strcpy(errorMessage, "DDERR_NOCLIPPERATTACHED  ");
            break;
        default:
            strcpy(errorMessage, "Error type unknown");
            break;
    }
    MessageBeep(0);
    MessageBeep(0);
    MessageBeep(0);
    sprintf(
        gText,
        "DirectDraw Error:\n\n  '%s'\n\n  File: '%s'\n  Line: %d",
        errorMessage,
        file,
        line
    );
    LogStr(gText);
    ShutDown(gText);
}

void __cdecl DDUpdatePalette(i8* paletteData) {
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
    ProcessAssert(
        reinterpret_cast<i32>(lpDDPal),
        "wingraph.cpp",
        518
    );
    result0 = lpDDPal->SetEntries(
        0,
        WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE * (SYSTEM_PALETTE_REGION_COUNT),
        &LogicalPalette.entries[WINGRAPH_SYSTEM_PALETTE_SIZE]
    );
    if (result0 != DD_OK)
        DDSD(
            result0,
            "wingraph.cpp",
            522
        );
}

void DDCleanUpWinGraphics(void) {
    HRESULT restoreResult;
    HRESULT result;

    if (lpDD != NULL) {
        restoreResult = lpDD->RestoreDisplayMode();
        if (lpClipper != NULL) {
            if (lpDDSPrimary != NULL) {
                result = lpDDSPrimary->SetClipper(NULL);
                if (result != DD_OK && result != DDERR_NOCLIPPERATTACHED)
                    DDSD(
                        result,
                        "wingraph.cpp",
                        540
                    );
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
            DDSD(
                result,
                "wingraph.cpp",
                564
            );
        lpDD->Release();
        lpDD = NULL;
    }
}

void DDSetFullScreenStatus(i32 fullScreen) {
    i32 width;
    i32 windowHeight;
    i32 x;
    i32 y;
    HRESULT hres;

    if (gbWinGraphBusy != 0)
        return;
    if (gConfig.gfx[(giCurExe)].fullScreen == fullScreen)
        return;
    {
        x = gConfig.gfx[(giCurExe)].x;
        y = gConfig.gfx[(giCurExe)].y;
        width = gConfig.gfx[(giCurExe)].width;
        windowHeight = gConfig.gfx[(giCurExe)].height;
        gbWinGraphBusy = true;
        gConfig.gfx[(giCurExe)].fullScreen = fullScreen;
        if (gConfig.gfx[(giCurExe)].fullScreen != 0)
            SetMenuStatus(0);

        hres = lpDD->SetCooperativeLevel(
            hwndApp,
            DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT
        );
        if (hres != DD_OK)
            DDSD(
                hres,
                "wingraph.cpp",
                593
            );
        if (gConfig.gfx[(giCurExe)].fullScreen != 0) {
            hres = lpDD->SetDisplayMode(WINGRAPH_WIDTH, WINGRAPH_HEIGHT, WINGRAPH_COLOR_DEPTH);
            if (hres != DD_OK)
                DDSD(
                    hres,
                    "wingraph.cpp",
                    599
                );
        } else {
            hres = lpDD->RestoreDisplayMode();
            if (hres != DD_OK)
                DDSD(
                    hres,
                    "wingraph.cpp",
                    606
                );
            hres = lpDD->SetCooperativeLevel(hwndApp, DDSCL_NORMAL);
            if (hres != DD_OK)
                DDSD(
                    hres,
                    "wingraph.cpp",
                    611
                );
        }
        if (lpDDSPrimary != NULL) {
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        }
        CreatePrimary();
        hres = lpDDSPrimary->SetPalette(lpDDPal);
        if (hres != DD_OK)
            DDSD(
                hres,
                "wingraph.cpp",
                623
            );
        WritePrefs();
        gbWinGraphBusy = false;
        if (gConfig.gfx[(giCurExe)].fullScreen == 0) {
            SetMenuStatus(1);
            ResizeWindow(x, y, width, windowHeight);
        } else {
            gConfig.gfx[(giCurExe)].x = x;
            gConfig.gfx[(giCurExe)].y = y;
            gConfig.gfx[(giCurExe)].width = width;
            gConfig.gfx[(giCurExe)].height = windowHeight;
        }
        SetupClipper();
    }
}

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

void __cdecl WGUpdatePalette(i8* paletteData) {
    HDC dc0;
    i32 result;
    i32 idx;

    for (idx = WINGRAPH_SYSTEM_PALETTE_SIZE;
         idx < WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE;
         idx++) {
        LogicalPalette.entries[idx].peRed =
            paletteData[idx * PALETTE_COMPONENT_COUNT + PALETTE_RED_COMPONENT]
            << PALETTE_VALUE_SHIFT;
        screenImage.colors[idx].rgbRed = LogicalPalette.entries[idx].peRed;
        LogicalPalette.entries[idx].peGreen =
            paletteData[idx * PALETTE_COMPONENT_COUNT + PALETTE_GREEN_COMPONENT]
            << PALETTE_VALUE_SHIFT;
        screenImage.colors[idx].rgbGreen = LogicalPalette.entries[idx].peGreen;
        LogicalPalette.entries[idx].peBlue =
            paletteData[idx * PALETTE_COMPONENT_COUNT + PALETTE_BLUE_COMPONENT]
            << PALETTE_VALUE_SHIFT;
        screenImage.colors[idx].rgbBlue = LogicalPalette.entries[idx].peBlue;
    }
    AnimatePalette(
        hpalApp,
        WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE * (SYSTEM_PALETTE_REGION_COUNT),
        &LogicalPalette.entries[WINGRAPH_SYSTEM_PALETTE_SIZE]
    );
    WinGSetDIBColorTable(
        hdcImage,
        WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE * (SYSTEM_PALETTE_REGION_COUNT),
        &screenImage.colors[WINGRAPH_SYSTEM_PALETTE_SIZE]
    );
    if (hpalApp != NULL)
        DeleteObject(hpalApp);
    hpalApp = CreatePalette(reinterpret_cast<LOGPALETTE*>(&LogicalPalette));
    dc0 = GetDC(hwndApp);
    if (hpalApp != NULL)
        SelectPalette(dc0, hpalApp, 0);
    result = RealizePalette(dc0);
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

void WGInitializePalette(void) {
    HDC dc0;
    i32 entry0;

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
    for (entry0 = 0; entry0 < WINGRAPH_SYSTEM_PALETTE_SIZE; entry0++) {
        screenImage.colors[entry0].rgbRed = LogicalPalette.entries[entry0].peRed;
        screenImage.colors[entry0].rgbGreen = LogicalPalette.entries[entry0].peGreen;
        screenImage.colors[entry0].rgbBlue = LogicalPalette.entries[entry0].peBlue;
        screenImage.colors[entry0].rgbReserved = 0;
        LogicalPalette.entries[entry0].peFlags = 0;
        screenImage.colors[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry0].rgbRed =
            LogicalPalette.entries[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry0]
                .peRed;
        screenImage.colors[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry0].rgbGreen =
            LogicalPalette.entries[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry0]
                .peGreen;
        screenImage.colors[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry0].rgbBlue =
            LogicalPalette.entries[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry0]
                .peBlue;
        screenImage.colors[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry0]
            .rgbReserved = 0;
        LogicalPalette.entries[WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE + entry0]
            .peFlags = 0;
    }
    for (entry0 = WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry0 < WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry0++) {
        LogicalPalette.entries[entry0].peRed = 0;
        screenImage.colors[entry0].rgbRed = 0;
        LogicalPalette.entries[entry0].peGreen = 0;
        screenImage.colors[entry0].rgbGreen = 0;
        LogicalPalette.entries[entry0].peBlue = 0;
        screenImage.colors[entry0].rgbBlue = 0;
        screenImage.colors[entry0].rgbReserved = 0;
        LogicalPalette.entries[entry0].peFlags = PC_NOCOLLAPSE;
    }
    hpalApp = CreatePalette(reinterpret_cast<LOGPALETTE*>(&LogicalPalette));
}

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

void ConnectToDLLs(void) {
    hDDrawLibrary = LoadLibraryA("DDRAW.DLL");
    if (reinterpret_cast<u32l>(hDDrawLibrary) >= WINGRAPH_LOAD_LIBRARY_SUCCESS) {
        lpDirectDrawCreate = reinterpret_cast<DirectDrawCreateProc>(
            GetProcAddress(
                hDDrawLibrary,
                "DirectDrawCreate"
            )
        );
    }
    if (lpDirectDrawCreate != NULL) {
        gbDDrawAttached = true;
    } else {
        gConfig.gfx[(giCurExe)].fullScreen = 0;
        SetMenuStatus(1);
    }
}

void DisconnectDLLs(void) {
    if (reinterpret_cast<u32l>(hDDrawLibrary) >= WINGRAPH_LOAD_LIBRARY_SUCCESS) {
        FreeLibrary(hDDrawLibrary);
    }
}

void RestoreDisplayMode(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING) {
        return;
    } else {
        DDRestoreDisplayMode();
    }
}

i32 SetPalette(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        return 0;
    else
        return DDSetPalette();
}

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
                "Для Героев II требуется "
                    "режим в 256 цветов или "
                    "больше.\n\nЧтобы "
                    "изменить режим "
                    "цветности, щелкните "
                    "правой кнопкой по "
                    "рабочему столу Windows и "
                    "выберите 'Properties'/Свойства'. "
                    "Затем в установках "
                    "выберите глубину "
                    "цвета."


            );
    }
}

void InitGraphics(void) {
    LogStr("IG1");
    ConnectToDLLs();
    LogStr("IG2");
    if (gConfig.gfx[(giCurExe)].fullScreen != 0)
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

i32 AppPaint(void* window, void* message) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        return WGAppPaint(window, message);
    else
        return DDAppPaint(window, message);
}

void InitializePalette(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        WGInitializePalette();
    else
        DDInitializePalette();
}

void __cdecl UpdatePalette(i8* paletteData) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        WGUpdatePalette(paletteData);
    else
        DDUpdatePalette(paletteData);
}

void CleanUpWinGraphics(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        WGCleanUpWinGraphics();
    else
        DDCleanUpWinGraphics();
    DisconnectDLLs();
}

void SetFullScreenStatus(i32 fullScreen) {
    if (gbInSmackMgr != 0)
        return;
    if (fullScreen == gConfig.gfx[(giCurExe)].fullScreen)
        return;
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING) {
        if (gbDDrawAttached == 0)
            return;
        gConfig.gfx[(giCurExe)].fullScreen = 1;
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

i32 QueryNewPalette(void) {
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        return WGQueryNewPalette();
    else
        return DDQueryNewPalette();
}

i32 SetGraphicsType(WingraphGraphicsType graphicsType) {
    i32 fullState;
    i32 x;
    i32 y;
    i32 width;
    i32 hgt;
    void* buffer;

    if (giGraphicsType == graphicsType)
        return 1;
    if (graphicsType == WINGRAPH_GRAPHICS_WING && gbWinGAttached == 0)
        return 0;
    if (graphicsType == WINGRAPH_GRAPHICS_DIRECT_DRAW && gbDDrawAttached == 0)
        return 0;

    fullState = gConfig.gfx[(giCurExe)].fullScreen;
    x = gConfig.gfx[(giCurExe)].x;
    y = gConfig.gfx[(giCurExe)].y;
    width = gConfig.gfx[(giCurExe)].width;
    hgt = gConfig.gfx[(giCurExe)].height;
    buffer = H2_ALLOC(WINGRAPH_WIDTH * WINGRAPH_HEIGHT);
    memcpy(buffer, gpWindowManager->m_screen->m_pixels, WINGRAPH_WIDTH * WINGRAPH_HEIGHT);
    if (graphicsType == WINGRAPH_GRAPHICS_WING) {
        gConfig.gfx[(giCurExe)].fullScreen = 0;
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
    memcpy(gpWindowManager->m_screen->m_pixels, buffer, WINGRAPH_WIDTH * WINGRAPH_HEIGHT);
    H2_FREE(buffer);
    if (fullState != 0 && graphicsType == WINGRAPH_GRAPHICS_WING) {
        SetMenuStatus(1);
        ResizeWindow(x, y, width, hgt);
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
