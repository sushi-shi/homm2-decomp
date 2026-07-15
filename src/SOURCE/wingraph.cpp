// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\wingraph.obj   from: (directly linked into exe)
// functions: 32   data: 25
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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

#define WINGRAPH_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\wingraph.cpp"

DATA(0x00525148) static RECT gDDSourceRect;
DATA(0x00525158) static RECT gDDClientRect;
DATA(0x00525170) static RECT gDDDestinationRect;
DATA(0x005255ac) static HRESULT gDDResult;
DATA(0x005255b0) static DDSURFACEDESC gDDSurfaceDesc;
DATA(0x004f16c4) static short gDDRestoreLineBase = 45;
DATA(0x004f16f4) static short gCreatePrimaryLineBase = 67;
DATA(0x004f1724) static short gSetupClipperLineBase = 87;
DATA(0x004f17ac) static short gDDInitLineBase = 110;
DATA(0x004f1864) static short gDDPaintLineBase = 157;
DATA(0x004f19b0) static short gDDInitializePaletteLineBase = 296;
DATA(0x004f19e0) static short gDDSetPaletteLineBase = 368;
DATA(0x004f1a10) static short gDDCreateSurfaceLineBase = 393;
DATA(0x004f1dbc) static short gDDUpdatePaletteLineBase = 500;
DATA(0x004f1e18) static short gDDCleanUpLineBase = 526;
DATA(0x004f1e74) static short gDDSetFullScreenLineBase = 572;
DATA(0x004f2084) static short gSetGraphicsTypeLineBase = 1247;

VA(0x00435290, 0x55)
void DDRestoreDisplayMode(void)
{
    HRESULT result;

    if (lpDD != 0) {
        result = lpDD->RestoreDisplayMode();
        if (result != DD_OK)
            DDSD(result, WINGRAPH_SOURCE_FILE, gDDRestoreLineBase + 7);
    }
}

VA(0x004352e5, 0x46)
int DDQueryNewPalette(void)
{
    int unused;

    if (gbWinGraphBusy != 0)
        return 1;
    if (gbForegroundApp == 0)
        return 1;
    return SetPalette();
}

VA(0x0043532b, 0x94)
void CreatePrimary(void)
{
    HRESULT result;

    lpDDSPrimary = DDCreateSurface(WINGRAPH_WIDTH, WINGRAPH_HEIGHT, 1);
    if (lpClipper != 0) {
        result = lpDDSPrimary->SetClipper(0);
        if (result != DD_OK && result != DDERR_NOCLIPPERATTACHED)
            DDSD(result, WINGRAPH_SOURCE_FILE, gCreatePrimaryLineBase + 10);
        lpClipper->Release();
        lpClipper = 0;
    }
}

VA(0x004353bf, 0xe3)
void SetupClipper(void)
{
    HRESULT result;

    if (gConfig.gfx[giCurExe].fullScreen == 0) {
        result = lpDD->CreateClipper(0, &lpClipper, 0);
        if (result != DD_OK)
            DDSD(result, WINGRAPH_SOURCE_FILE, gSetupClipperLineBase + 8);
        result = lpClipper->SetHWnd(0, hwndApp);
        if (result != DD_OK)
            DDSD(result, WINGRAPH_SOURCE_FILE, gSetupClipperLineBase + 13);
        result = lpDDSPrimary->SetClipper(lpClipper);
        if (result != DD_OK)
            DDSD(result, WINGRAPH_SOURCE_FILE, gSetupClipperLineBase + 18);
    }
}

VA(0x004354a2, 0x15f)
void DDInitGraphics(void)
{
    HRESULT result;

    if (gbWinGraphBusy != 0)
        return;
    result = lpDirectDrawCreate(0, &lpDD, 0);
    if (result != DD_OK)
        DDSD(result, WINGRAPH_SOURCE_FILE, gDDInitLineBase + 8);
    if (gConfig.gfx[giCurExe].fullScreen != 0) {
        SetMenuStatus(0);
        result = lpDD->SetCooperativeLevel(hwndApp,
            DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT);
        if (result != DD_OK)
            DDSD(result, WINGRAPH_SOURCE_FILE, gDDInitLineBase + 20);
        result = lpDD->SetDisplayMode(
            WINGRAPH_WIDTH, WINGRAPH_HEIGHT, WINGRAPH_COLOR_DEPTH);
        if (result != DD_OK)
            DDSD(result, WINGRAPH_SOURCE_FILE, gDDInitLineBase + 24);
    } else {
        result = lpDD->SetCooperativeLevel(hwndApp, DDSCL_NORMAL);
        if (result != DD_OK)
            DDSD(result, WINGRAPH_SOURCE_FILE, gDDInitLineBase + 31);
    }
    CreatePrimary();
    SetupClipper();
    lpDDSOne = DDCreateSurface(WINGRAPH_WIDTH, WINGRAPH_HEIGHT, 0);
    InitializePalette();
}

VA(0x00435601, 0x530)
int DDAppPaint(void *window, void *paintDC)
{
    int sourceTop4;
    int sourceWidth5;
    int sourceHeight3;
    int sourceLeft3;
    POINT point0;
    PAINTSTRUCT paint3;

    if (gbWinGraphBusy != 0)
        return 1;
    if (gbMinimized != 0)
        return 1;
    if (lpDD == 0)
        return 1;
    {
        gbWinGraphBusy = 1;
        paintDC = BeginPaint(reinterpret_cast<HWND>(window), &paint3);
        GetClientRect(reinterpret_cast<HWND>(window), &gDDClientRect);
        if (paint3.rcPaint.right == 0 || paint3.rcPaint.bottom == 0)
            paint3.rcPaint = gDDClientRect;
        if (paint3.rcPaint.right < WINGRAPH_WIDTH)
            paint3.rcPaint.right++;
        if (paint3.rcPaint.bottom < WINGRAPH_HEIGHT)
            paint3.rcPaint.bottom++;

        gDDDestinationRect = paint3.rcPaint;
        sourceWidth5 = ((gDDDestinationRect.right -
            gDDDestinationRect.left + 1) *
            WINGRAPH_WIDTH) / iMainWinScreenWidth;
        sourceHeight3 = ((gDDDestinationRect.bottom -
            gDDDestinationRect.top + 1) *
            WINGRAPH_HEIGHT) / iMainWinScreenHeight;
        sourceLeft3 = (gDDDestinationRect.left * WINGRAPH_WIDTH) /
            iMainWinScreenWidth;
        sourceTop4 = (gDDDestinationRect.top * WINGRAPH_HEIGHT) /
            iMainWinScreenHeight;
        if (giScrollX != 0) {
            sourceLeft3 = giScrollX + WINGRAPH_SCROLL_MARGIN;
            sourceWidth5 = WINGRAPH_SCROLL_SIZE;
        }
        if (giScrollY != 0) {
            sourceTop4 = giScrollY + WINGRAPH_SCROLL_MARGIN;
            sourceHeight3 = WINGRAPH_SCROLL_SIZE;
        }
        gDDSourceRect.left = sourceLeft3;
        gDDSourceRect.right = 0[&sourceWidth5] + sourceLeft3 - 1;
        gDDSourceRect.top = sourceTop4;
        gDDSourceRect.bottom = sourceTop4 + sourceHeight3 - 1;

        point0.y = 0;
        point0.x = point0.y;
        ClientToScreen(hwndApp, &point0);
        OffsetRect(&gDDDestinationRect, point0.x, point0.y);
        gDDResult = lpDDSOne->Unlock(0);
        if (gDDResult != DD_OK)
            DDSD(gDDResult, WINGRAPH_SOURCE_FILE, gDDPaintLineBase + 71);

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
            gDDResult = lpDDSPrimary->Blt(&gDDDestinationRect, lpDDSOne,
                &gDDSourceRect, DDBLT_WAIT, 0);
            if (gDDResult == DDERR_SURFACELOST) {
                gDDResult = lpDDSPrimary->Restore();
                if (gDDResult == DDERR_WRONGMODE) {
                    LogStr("ResetDisplayMode");
                    gDDResult = lpDD->SetDisplayMode(WINGRAPH_WIDTH,
                        WINGRAPH_HEIGHT, WINGRAPH_COLOR_DEPTH);
                    if (gDDResult != DD_OK)
                        DDSD(gDDResult, WINGRAPH_SOURCE_FILE, gDDPaintLineBase + 91);
                    gDDResult = lpDDSPrimary->Restore();
                    if (gDDResult != DD_OK)
                        DDSD(gDDResult, WINGRAPH_SOURCE_FILE, gDDPaintLineBase + 95);
                    gDDDestinationRect = gDDSourceRect;
                }
                if (gDDResult != DD_OK)
                    DDSD(gDDResult, WINGRAPH_SOURCE_FILE, gDDPaintLineBase + 103);
            } else if (gDDResult == DDERR_SURFACEBUSY &&
                       KBTickCount() < lPaintStart + WINGRAPH_PAINT_TIMEOUT) {
                iBusyRetry++;
            } else if (gDDResult != DD_OK) {
                DDSD(gDDResult, WINGRAPH_SOURCE_FILE, gDDPaintLineBase + 108);
            } else {
                break;
            }
        }

        memset(&gDDSurfaceDesc, 0, sizeof(gDDSurfaceDesc));
        gDDSurfaceDesc.dwSize = sizeof(gDDSurfaceDesc);
        gDDResult = lpDDSOne->Lock(0, &gDDSurfaceDesc, DDLOCK_WAIT, 0);
        if (gDDResult != DD_OK)
            DDSD(gDDResult, WINGRAPH_SOURCE_FILE, gDDPaintLineBase + 118);
        if (gpWindowManager->m_screen != 0) {
            gpWindowManager->m_screen->m_pixels =
                static_cast<unsigned char *>(gDDSurfaceDesc.lpSurface);
            lpInitWin = gDDSurfaceDesc.lpSurface;
        } else {
            lpInitWin = gDDSurfaceDesc.lpSurface;
        }
        if (gDDResult != DD_OK)
            DDSD(gDDResult, WINGRAPH_SOURCE_FILE, gDDPaintLineBase + 129);
        EndPaint(reinterpret_cast<HWND>(window), &paint3);
        gbWinGraphBusy = 0;
    }
    return 1;
}

VA(0x00435b31, 0x13c)
void DDInitializePalette(void)
{
    HDC dc0;
    int entry;
    HRESULT result0;

    if (gbWinGraphBusy != 0)
        return;
    {
        dc0 = GetDC(0);
        GetSystemPaletteEntries(dc0, 0, WINGRAPH_SYSTEM_PALETTE_SIZE,
            LogicalPalette.entries);
        GetSystemPaletteEntries(dc0,
            WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE,
            WINGRAPH_SYSTEM_PALETTE_SIZE,
            &LogicalPalette.entries[WINGRAPH_PALETTE_SIZE -
                WINGRAPH_SYSTEM_PALETTE_SIZE]);
        ReleaseDC(0, dc0);
        for (entry = 0; entry < WINGRAPH_SYSTEM_PALETTE_SIZE; entry++) {
            LogicalPalette.entries[entry].peFlags = 0;
            LogicalPalette.entries[WINGRAPH_PALETTE_SIZE -
                WINGRAPH_SYSTEM_PALETTE_SIZE + entry].peFlags = 0;
        }
        for (entry = WINGRAPH_SYSTEM_PALETTE_SIZE;
             entry < WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE;
             entry++) {
            LogicalPalette.entries[entry].peRed = 0;
            LogicalPalette.entries[entry].peGreen = 0;
            LogicalPalette.entries[entry].peBlue = 0;
            LogicalPalette.entries[entry].peFlags = PC_NOCOLLAPSE;
        }
        result0 = lpDD->CreatePalette(DDPCAPS_8BIT, LogicalPalette.entries,
            &lpDDPal, 0);
        if (result0 != DD_OK)
            DDSD(result0, WINGRAPH_SOURCE_FILE,
                gDDInitializePaletteLineBase + 63);
        SetPalette();
    }
}

VA(0x00435c6d, 0xaf)
int DDSetPalette(void)
{
    HRESULT result;

    if (gbWinGraphBusy != 0)
        return 1;
    if (gbForegroundApp == 0)
        return 1;
    if (lpDDPal == 0 || lpDDSPrimary == 0 || lpDD == 0)
        return 1;
    result = lpDDSPrimary->SetPalette(lpDDPal);
    if (result != DD_OK)
        DDSD(result, WINGRAPH_SOURCE_FILE, gDDSetPaletteLineBase + 15);
    return 0;
}

VA(0x00435d1c, 0x133)
struct IDirectDrawSurface * DDCreateSurface(
    unsigned long width, unsigned long height, int primary)
{
    DDSURFACEDESC description;
    IDirectDrawSurface *surface;
    int cnt;
    int unused;
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
    rv = lpDD->CreateSurface(&description, &surface, 0);
    if (rv != DD_OK)
        DDSD(rv, WINGRAPH_SOURCE_FILE, gDDCreateSurfaceLineBase + 28);
    if (primary == 0) {
        rv = surface->Lock(0, &description, DDLOCK_WAIT, 0);
        if (rv != DD_OK)
            DDSD(rv, WINGRAPH_SOURCE_FILE,
                gDDCreateSurfaceLineBase + 36);
        if (gpWindowManager->m_screen != 0) {
            gpWindowManager->m_screen->m_pixels =
                static_cast<unsigned char *>(description.lpSurface);
            lpInitWin = description.lpSurface;
        } else {
            lpInitWin = description.lpSurface;
        }
    }
    return surface;
}

// @early-stop
// reloc-masked: all 1450 non-table bytes are identical; 0x52c..0x553 is a
// delinked local-label jump table. Only local/constant relocation names differ;
// every external relocation target agrees.
VA(0x00435e4f, 0x5d2)
void DDSD(int error, char *file, int line)
{
    HRESULT restoreResult;
    int unused;
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
    sprintf(gText,
        "DirectDraw Error:\n\n  '%s'\n\n  File: '%s'\n\n  Line: %d",
        errorMessage0, file, line);
    LogStr(gText);
    ShutDown(gText);
}

VA(0x00436421, 0x118)
void DDUpdatePalette(signed char *paletteData)
{
    int entry;
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
    ProcessAssert(reinterpret_cast<int>(lpDDPal), WINGRAPH_SOURCE_FILE,
        gDDUpdatePaletteLineBase + 18);
    result0 = lpDDPal->SetEntries(0, WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE * 2,
        &LogicalPalette.entries[WINGRAPH_SYSTEM_PALETTE_SIZE]);
    if (result0 != DD_OK)
        DDSD(result0, WINGRAPH_SOURCE_FILE, gDDUpdatePaletteLineBase + 22);
}

VA(0x00436539, 0x177)
void DDCleanUpWinGraphics(void)
{
    HRESULT restoreResult;
    HRESULT result;

    if (lpDD != 0) {
        restoreResult = lpDD->RestoreDisplayMode();
        if (lpClipper != 0) {
            if (lpDDSPrimary != 0) {
                result = lpDDSPrimary->SetClipper(0);
                if (result != DD_OK && result != DDERR_NOCLIPPERATTACHED)
                    DDSD(result, WINGRAPH_SOURCE_FILE, gDDCleanUpLineBase + 14);
            }
            lpClipper->Release();
            lpClipper = 0;
        }
        if (lpDDSPrimary != 0) {
            lpDDSPrimary->Release();
            lpDDSPrimary = 0;
        }
        if (lpDDSOne != 0) {
            lpDDSOne->Release();
            lpDDSOne = 0;
        }
        if (lpDDPal != 0) {
            lpDDPal->Release();
            lpDDPal = 0;
        }
        result = lpDD->SetCooperativeLevel(hwndApp, DDSCL_NORMAL);
        if (result != DD_OK)
            DDSD(result, WINGRAPH_SOURCE_FILE, gDDCleanUpLineBase + 38);
        lpDD->Release();
        lpDD = 0;
    }
}

VA(0x004366b0, 0x305)
void DDSetFullScreenStatus(int fullScreen)
{
    int width;
    int windowHeight0;
    int x;
    int y;
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
        gbWinGraphBusy = 1;
        gConfig.gfx[giCurExe].fullScreen = fullScreen;
        if (gConfig.gfx[giCurExe].fullScreen != 0)
            SetMenuStatus(0);

        result0 = lpDD->SetCooperativeLevel(hwndApp,
            DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT);
        if (result0 != DD_OK)
            DDSD(result0, WINGRAPH_SOURCE_FILE,
                gDDSetFullScreenLineBase + 21);
        if (gConfig.gfx[giCurExe].fullScreen != 0) {
            result0 = lpDD->SetDisplayMode(
                WINGRAPH_WIDTH, WINGRAPH_HEIGHT, WINGRAPH_COLOR_DEPTH);
            if (result0 != DD_OK)
                DDSD(result0, WINGRAPH_SOURCE_FILE,
                    gDDSetFullScreenLineBase + 27);
        } else {
            result0 = lpDD->RestoreDisplayMode();
            if (result0 != DD_OK)
                DDSD(result0, WINGRAPH_SOURCE_FILE,
                    gDDSetFullScreenLineBase + 34);
            result0 = lpDD->SetCooperativeLevel(hwndApp, DDSCL_NORMAL);
            if (result0 != DD_OK)
                DDSD(result0, WINGRAPH_SOURCE_FILE,
                    gDDSetFullScreenLineBase + 39);
        }
        if (lpDDSPrimary != 0) {
            lpDDSPrimary->Release();
            lpDDSPrimary = 0;
        }
        CreatePrimary();
        result0 = lpDDSPrimary->SetPalette(lpDDPal);
        if (result0 != DD_OK)
            DDSD(result0, WINGRAPH_SOURCE_FILE,
                gDDSetFullScreenLineBase + 51);
        WritePrefs();
        gbWinGraphBusy = 0;
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
int WGQueryNewPalette(void)
{
    int paletteChanges;
    HDC hdc;

    hdc = GetDC(hwndApp);
    if (hpalApp != 0)
        SelectPalette(hdc, hpalApp, 0);
    paletteChanges = RealizePalette(hdc);
    ReleaseDC(hwndApp, hdc);
    if (paletteChanges > 0) {
        InvalidateRect(hwndApp, 0, 1);
        return 1;
    } else {
        return 0;
    }
}

VA(0x00436a3e, 0x151)
void WGInitGraphics(void)
{
    HBITMAP bitmap;

    if (hdcImage != 0)
        return;
    if (WinGRecommendDIBFormat(
            reinterpret_cast<BITMAPINFO *>(&screenImage)) != 0) {
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
    bitmap = WinGCreateBitmap(hdcImage,
        reinterpret_cast<BITMAPINFO *>(&screenImage), &screenImage.bits);
    screenImage.header.biSizeImage =
        (screenImage.header.biWidth | 0) * screenImage.header.biHeight;
    screenImage.header.biSizeImage *= Orientation;
    gbmOldMonoBitmap = static_cast<HBITMAP>(SelectObject(hdcImage, bitmap));
    lpInitWin = screenImage.bits;
    PatBlt(hdcImage, 0, 0, iMainWinScreenWidth, iMainWinScreenHeight,
        BLACKNESS);
}

VA(0x00436b8f, 0x1e7)
void WGUpdatePalette(signed char *paletteData)
{
    HDC dc0;
    int entry;
    int result0;

    for (entry = WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry < WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry++) {
        LogicalPalette.entries[entry].peRed = paletteData[entry * 3] << 2;
        screenImage.colors[entry].rgbRed =
            LogicalPalette.entries[entry].peRed;
        LogicalPalette.entries[entry].peGreen =
            paletteData[entry * 3 + 1] << 2;
        screenImage.colors[entry].rgbGreen =
            LogicalPalette.entries[entry].peGreen;
        LogicalPalette.entries[entry].peBlue =
            paletteData[entry * 3 + 2] << 2;
        screenImage.colors[entry].rgbBlue =
            LogicalPalette.entries[entry].peBlue;
    }
    AnimatePalette(hpalApp, WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE * 2,
        &LogicalPalette.entries[WINGRAPH_SYSTEM_PALETTE_SIZE]);
    WinGSetDIBColorTable(hdcImage, WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE * 2,
        &screenImage.colors[WINGRAPH_SYSTEM_PALETTE_SIZE]);
    if (hpalApp != 0)
        DeleteObject(hpalApp);
    hpalApp = CreatePalette(
        reinterpret_cast<LOGPALETTE *>(&LogicalPalette));
    dc0 = GetDC(hwndApp);
    if (hpalApp != 0)
        SelectPalette(dc0, hpalApp, 0);
    result0 = RealizePalette(dc0);
    ReleaseDC(hwndApp, dc0);
    if (giMainVideoModeColorDepth != WINGRAPH_COLOR_DEPTH &&
        gpWindowManager->m_screen != 0) {
        if (gbLimitedCombatUpdatePalette != 0) {
            if (gbFullCombatScreenDrawn != 0)
                BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0,
                    WINGRAPH_WIDTH, WINGRAPH_LIMITED_COMBAT_HEIGHT, 0, 0);
        } else {
            BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0,
                WINGRAPH_WIDTH, WINGRAPH_HEIGHT, 0, 0);
        }
    }
}

VA(0x00436d76, 0x1d5)
void WGInitializePalette(void)
{
    HDC dc0;
    int entry;

    if (hpalApp != 0)
        return;
    dc0 = GetDC(0);
    GetSystemPaletteEntries(dc0, 0, WINGRAPH_SYSTEM_PALETTE_SIZE,
        LogicalPalette.entries);
    GetSystemPaletteEntries(dc0,
        WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE,
        WINGRAPH_SYSTEM_PALETTE_SIZE,
        &LogicalPalette.entries[WINGRAPH_PALETTE_SIZE -
            WINGRAPH_SYSTEM_PALETTE_SIZE]);
    ReleaseDC(0, dc0);
    for (entry = 0; entry < WINGRAPH_SYSTEM_PALETTE_SIZE; entry++) {
        screenImage.colors[entry].rgbRed =
            LogicalPalette.entries[entry].peRed;
        screenImage.colors[entry].rgbGreen =
            LogicalPalette.entries[entry].peGreen;
        screenImage.colors[entry].rgbBlue =
            LogicalPalette.entries[entry].peBlue;
        screenImage.colors[entry].rgbReserved = 0;
        LogicalPalette.entries[entry].peFlags = 0;
        screenImage.colors[WINGRAPH_PALETTE_SIZE -
            WINGRAPH_SYSTEM_PALETTE_SIZE + entry].rgbRed =
            LogicalPalette.entries[WINGRAPH_PALETTE_SIZE -
                WINGRAPH_SYSTEM_PALETTE_SIZE + entry].peRed;
        screenImage.colors[WINGRAPH_PALETTE_SIZE -
            WINGRAPH_SYSTEM_PALETTE_SIZE + entry].rgbGreen =
            LogicalPalette.entries[WINGRAPH_PALETTE_SIZE -
                WINGRAPH_SYSTEM_PALETTE_SIZE + entry].peGreen;
        screenImage.colors[WINGRAPH_PALETTE_SIZE -
            WINGRAPH_SYSTEM_PALETTE_SIZE + entry].rgbBlue =
            LogicalPalette.entries[WINGRAPH_PALETTE_SIZE -
                WINGRAPH_SYSTEM_PALETTE_SIZE + entry].peBlue;
        screenImage.colors[WINGRAPH_PALETTE_SIZE -
            WINGRAPH_SYSTEM_PALETTE_SIZE + entry].rgbReserved = 0;
        LogicalPalette.entries[WINGRAPH_PALETTE_SIZE -
            WINGRAPH_SYSTEM_PALETTE_SIZE + entry].peFlags = 0;
    }
    for (entry = WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry < WINGRAPH_PALETTE_SIZE - WINGRAPH_SYSTEM_PALETTE_SIZE;
         entry++) {
        LogicalPalette.entries[entry].peRed = 0;
        screenImage.colors[entry].rgbRed =
            LogicalPalette.entries[entry].peRed;
        LogicalPalette.entries[entry].peGreen = 0;
        screenImage.colors[entry].rgbGreen =
            LogicalPalette.entries[entry].peGreen;
        LogicalPalette.entries[entry].peBlue = 0;
        screenImage.colors[entry].rgbBlue =
            LogicalPalette.entries[entry].peBlue;
        screenImage.colors[entry].rgbReserved = 0;
        LogicalPalette.entries[entry].peFlags = PC_NOCOLLAPSE;
    }
    hpalApp = CreatePalette(
        reinterpret_cast<LOGPALETTE *>(&LogicalPalette));
}

VA(0x00436f4b, 0x1c4)
int WGAppPaint(void *window, void *paintDC)
{
    int padding8;
    RECT clientRect16;
    char unusedByte0;
    int destY1;
    int sourceX7;
    PAINTSTRUCT paint5;
    int destHeight3;
    int destWidth0;
    int destX7;
    int sourceY7;

    unusedByte0 = 0;
    if (screenImage.bits != 0) {
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
        if (iMainWinScreenWidth == WINGRAPH_WIDTH &&
            iMainWinScreenHeight == WINGRAPH_HEIGHT) {
            destX7 = paint5.rcPaint.left & WINGRAPH_PAINT_ALIGN_MASK;
            destWidth0 = paint5.rcPaint.right - destX7 + 1;
            destY1 = paint5.rcPaint.top;
            destHeight3 = paint5.rcPaint.bottom - destY1 + 1;
            WinGBitBlt(static_cast<HDC>(paintDC), destX7, destY1,
                destWidth0, destHeight3, hdcImage, giScrollX + destX7,
                0[&giScrollY] + destY1);
        } else {
            WinGStretchBlt(static_cast<HDC>(paintDC), destX7, destY1,
                destWidth0, destHeight3, hdcImage, sourceX7, sourceY7,
                (destWidth0 * WINGRAPH_WIDTH) / iMainWinScreenWidth,
                (destHeight3 * WINGRAPH_HEIGHT) / iMainWinScreenHeight);
        }
        EndPaint(reinterpret_cast<HWND>(window), &paint5);
    }
    return 1;
}

VA(0x0043710f, 0x78)
void WGCleanUpWinGraphics(void)
{
    HGDIOBJ bitmap;

    if (hdcImage != 0) {
        bitmap = SelectObject(hdcImage, gbmOldMonoBitmap);
        DeleteObject(bitmap);
        DeleteDC(hdcImage);
        hdcImage = 0;
    }
    if (hpalApp != 0) {
        DeleteObject(hpalApp);
        hpalApp = 0;
    }
}

VA(0x00437187, 0x80)
void ConnectToDLLs(void)
{
    hDDrawLibrary = LoadLibraryA("DDRAW.DLL");
    if (reinterpret_cast<unsigned long>(hDDrawLibrary) >=
        WINGRAPH_LOAD_LIBRARY_SUCCESS) {
        lpDirectDrawCreate = reinterpret_cast<DirectDrawCreateProc>(
            GetProcAddress(hDDrawLibrary, "DirectDrawCreate"));
    }
    if (lpDirectDrawCreate != 0) {
        gbDDrawAttached = 1;
    } else {
        gConfig.gfx[giCurExe].fullScreen = 0;
        SetMenuStatus(1);
    }
}

VA(0x00437207, 0x29)
void DisconnectDLLs(void)
{
    if (reinterpret_cast<unsigned long>(hDDrawLibrary) >=
        WINGRAPH_LOAD_LIBRARY_SUCCESS) {
        FreeLibrary(hDDrawLibrary);
    }
}

VA(0x00437230, 0x2c)
void RestoreDisplayMode(void)
{
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING) {
        return;
    } else {
        DDRestoreDisplayMode();
    }
}

VA(0x0043725c, 0x2e)
int SetPalette(void)
{
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        return 0;
    else
        return DDSetPalette();
}

VA(0x0043728a, 0x7e)
void GetGraphicsInfo(void)
{
    HDC screenDC;

    screenDC = GetDC(0);
    if (screenDC != 0) {
        giMainVideoModeColorDepth = GetDeviceCaps(screenDC, BITSPIXEL);
        giMainVideoModeWidth = GetDeviceCaps(screenDC, HORZRES);
        giMainVideoModeHeight = GetDeviceCaps(screenDC, VERTRES);
        ReleaseDC(0, screenDC);
        if (giMainVideoModeColorDepth < WINGRAPH_COLOR_DEPTH)
            ShutDown("Heroes II requires 256 color mode or higher.");
    }
}

VA(0x00437308, 0xa0)
void InitGraphics(void)
{
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
int AppPaint(void *window, void *message)
{
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        return WGAppPaint(window, message);
    else
        return DDAppPaint(window, message);
}

VA(0x004373ee, 0x2c)
void InitializePalette(void)
{
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        WGInitializePalette();
    else
        DDInitializePalette();
}

VA(0x0043741a, 0x38)
void UpdatePalette(signed char *paletteData)
{
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        WGUpdatePalette(paletteData);
    else
        DDUpdatePalette(paletteData);
}

VA(0x00437452, 0x31)
void CleanUpWinGraphics(void)
{
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        WGCleanUpWinGraphics();
    else
        DDCleanUpWinGraphics();
    DisconnectDLLs();
}

// @early-stop
// Relocation-masked code bytes and the 0x4 frame are identical. The only two
// objdiff rows are gConfig+0x30 versus retail's delinked const_00128d50 label;
// both indexed operands resolve to VA 0x00528d50 and all 13 relocations align.
VA(0x00437483, 0xe1)
void SetFullScreenStatus(int fullScreen)
{
    if (gbInSmackMgr != 0) {
    } else {
        if (gConfig.gfx[giCurExe].fullScreen == fullScreen) {
        } else {
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
    }
}

VA(0x00437564, 0x31)
int QueryNewPalette(void)
{
    if (giGraphicsType == WINGRAPH_GRAPHICS_WING)
        return WGQueryNewPalette();
    else
        return DDQueryNewPalette();
}

// @match-note
// The real body is target +0x000..+0x219; frame 0x1c, all six local slots, CFG,
// calls, and external relocations align. First code divergence is target +0x0c
// `mov eax,[ebp-0x1c]; cmp [giGraphicsType],eax` versus ours `mov eax,
// [giGraphicsType]; cmp [ebp-0x1c],eax` (one byte shorter). Equality operands
// were swapped and direct/addressed parameter spellings were tried. Target
// +0x21a..+0x285 is a delinked SVSearchArray ctor/dtor/atexit tail, not this body.
// Revisit after the SOURCE placeholder census reaches zero.
VA(0x00437595, 0x286)
int SetGraphicsType(int graphicsType)
{
    int fullScreen;
    int x;
    int y;
    int width;
    int height7;
    void *screenBuffer;

    if (graphicsType == giGraphicsType)
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
    screenBuffer = BaseAlloc(WINGRAPH_WIDTH * WINGRAPH_HEIGHT,
        WINGRAPH_SOURCE_FILE, gSetGraphicsTypeLineBase + 18);
    memcpy(screenBuffer, gpWindowManager->m_screen->m_pixels,
        WINGRAPH_WIDTH * WINGRAPH_HEIGHT);
    if (graphicsType == WINGRAPH_GRAPHICS_WING) {
        gConfig.gfx[giCurExe].fullScreen = 0;
        DDCleanUpWinGraphics();
        giGraphicsType = WINGRAPH_GRAPHICS_WING;
        WGInitGraphics();
        gpWindowManager->m_screen->m_pixels =
            static_cast<unsigned char *>(lpInitWin);
    } else {
        WGCleanUpWinGraphics();
        giGraphicsType = WINGRAPH_GRAPHICS_DIRECT_DRAW;
        DDInitGraphics();
        gpWindowManager->m_screen->m_pixels =
            static_cast<unsigned char *>(lpInitWin);
    }
    memcpy(gpWindowManager->m_screen->m_pixels, screenBuffer,
        WINGRAPH_WIDTH * WINGRAPH_HEIGHT);
    BaseFree(screenBuffer, WINGRAPH_SOURCE_FILE,
        gSetGraphicsTypeLineBase + 39);
    if (fullScreen != 0 && graphicsType == WINGRAPH_GRAPHICS_WING) {
        SetMenuStatus(1);
        ResizeWindow(x, y, width, height7);
    }
    BlitBitmapToScreen(gpWindowManager->m_screen, 0, 0, WINGRAPH_WIDTH,
        WINGRAPH_HEIGHT, 0, 0);
    UpdatePalette(gpBufferPalette->m_data);
    return 1;
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f1288) int gbWinGAttached = 1;
DATA(0x004f128c) int gbDDrawAttached = 0;
DATA(0x004f1290) int giGraphicsType = 1;
DATA(0x004f1294) long Orientation = 1;
DATA(0x004f1298) struct _PALETTE LogicalPalette = {
    WINGRAPH_PALETTE_VERSION, WINGRAPH_PALETTE_SIZE
};
DATA(0x004f169c) void *lpInitWin = 0;
DATA(0x004f16a0) int bPaletteInitialized = 0;
DATA(0x004f16a4) int giTtlBlts = 0;
DATA(0x004f16a8) int gbWinGraphBusy = 0;
DATA(0x004f16ac) DirectDrawCreateProc lpDirectDrawCreate = 0;
DATA(0x004f16b0) struct IDirectDraw *lpDD = 0;
DATA(0x004f16b4) struct IDirectDrawSurface *lpDDSPrimary = 0;
DATA(0x004f16b8) struct IDirectDrawSurface *lpDDSOne = 0;
DATA(0x004f16bc) struct IDirectDrawClipper *lpClipper = 0;
DATA(0x004f16c0) struct IDirectDrawPalette *lpDDPal = 0;
DATA(0x004f17b0) int iBusyRetry = 0;
DATA(0x004f1a14) int bInDDSD = 0;
DATA(0x004f1e78) HDC hdcImage = 0;
DATA(0x004f1e7c) HBITMAP gbmOldMonoBitmap = 0;
DATA(0x004f1e80) HPALETTE hpalApp = 0;
DATA(0x004f1f60) HINSTANCE hWinGLibrary = 0;
DATA(0x004f1f64) HINSTANCE hDDrawLibrary = 0;
DATA(0x004f2080) long lDelayRefresh = 0;
DATA(0x00525168) long lPaintStart;
DATA(0x00525180) struct _IMAGE screenImage;
