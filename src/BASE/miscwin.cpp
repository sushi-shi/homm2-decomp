#include <va.h>
#include <BASE/miscwin.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/X_GLOBAL.h>
#include <windows.h>
#include <string.h>

H2_ENUM_BEGIN(VesaBlitConstant)
    VESA_SCREEN_WIDTH    = 640,
    VESA_SCREEN_HEIGHT   = 480,
    ENLARGE_EXTENT_LIMIT = VESA_SCREEN_WIDTH - 3,
    ENLARGE_PIXEL_GROWTH = 4,
    NET_BOX_TOP          = 411
H2_ENUM_END(VesaBlitConstant)

VA(0x004d4610, 0x1e6)
extern "C" void __cdecl BlitBitmapToScreenVesa(
    bitmap* sourceBitmap,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height,
    i32 destinationX,
    i32 destinationY
) {
    if (sourceBitmap != gpWindowManager->m_screen) {
        i32 row;
        for (row = 0; row < height; row++)
            memcpy(
                gpWindowManager->m_screen->m_pixels
                    + (destinationY + row) * VESA_SCREEN_WIDTH + destinationX,
                sourceBitmap->m_pixels + (row + sourceY) * sourceBitmap->m_width + sourceX,
                width
            );
    }

    if (gbEnlargeScreenBlit != 0 && gConfig.gfx[IDX(giCurExe)].fullScreen == 0) {
        if (iMainWinScreenWidth == VESA_SCREEN_WIDTH
            && iMainWinScreenHeight == VESA_SCREEN_HEIGHT) {
            if (width < VESA_SCREEN_WIDTH)
                ++width;
            if (height < VESA_SCREEN_WIDTH)
                ++height;
        } else {
            if (destinationX > 0)
                --destinationX;
            if (destinationY > 0)
                --destinationY;
            if (width < ENLARGE_EXTENT_LIMIT)
                width += ENLARGE_PIXEL_GROWTH;
            if (height < ENLARGE_EXTENT_LIMIT)
                height += ENLARGE_PIXEL_GROWTH;
        }
    }

    if (gbLeaveNetBoxAlone != 0) {
        if (destinationY >= NET_BOX_TOP)
            return;
        if (destinationY + height >= NET_BOX_TOP)
            height = NET_BOX_TOP - destinationY;
    }

    RECT invalidRect;
    invalidRect.left = destinationX * iMainWinScreenWidth / VESA_SCREEN_WIDTH;
    invalidRect.top = destinationY * iMainWinScreenHeight / VESA_SCREEN_HEIGHT;
    invalidRect.right =
        (destinationX + width) * iMainWinScreenWidth / VESA_SCREEN_WIDTH - 1;
    invalidRect.bottom =
        (destinationY + height) * iMainWinScreenHeight / VESA_SCREEN_HEIGHT - 1;

    if (InvalidateRect(hwndApp, &invalidRect, 0) == 0)
        LogStr("InvalidateRect Failed");
    if (UpdateWindow(hwndApp) == 0)
        LogStr("UpdateWindow Failed");
}

VA(0x004d4800, 0x8)
i16 AutoInitSVGA(void) {
    return 0;
}

struct tagPAINTSTRUCT ps;
