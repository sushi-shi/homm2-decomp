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

VA(0x004d8540, 0x1e2)
extern "C" void __fastcall BlitBitmapToScreenVesa(
    bitmap* sourceBitmap,
    i32 sourceX,
    i32 sourceY,
    i32 width,
    i32 height,
    i32 destinationX,
    i32 destinationY
) {
    if (gpWindowManager->m_screen != sourceBitmap) {
        i32 row = 0;
        if (height > row) {
            i32 destinationOffset = destinationY * VESA_SCREEN_WIDTH;
            do {
                memcpy(
                    gpWindowManager->m_screen->m_pixels + destinationX + destinationOffset,
                    sourceBitmap->m_pixels + sourceX + (sourceY + row) * sourceBitmap->m_width,
                    width
                );
                destinationOffset += VESA_SCREEN_WIDTH;
                ++row;
            } while (row < height);
        }
    }

    if (gbEnlargeScreenBlit != 0 && gConfig.gfx[giCurExe].fullScreen == 0) {
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

VA(0x004d8730, 0x4)
i16 AutoInitSVGA(void) {
    return 0;
}

DATA(0x00538080) struct tagPAINTSTRUCT ps;
