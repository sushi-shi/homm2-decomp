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

// @semantic: First divergence is the destination-address association in the row copy: base adds the row offset before destinationX.
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
            i32 destinationOffset = destinationY * 640;
            do {
                memcpy(
                    gpWindowManager->m_screen->m_pixels + destinationX + destinationOffset,
                    sourceBitmap->m_pixels + sourceX + (sourceY + row) * sourceBitmap->m_width,
                    width
                );
                destinationOffset += 640;
                ++row;
            } while (row < height);
        }
    }

    if (gbEnlargeScreenBlit != 0 && gConfig.gfx[giCurExe].fullScreen == 0) {
        if (iMainWinScreenWidth == 640 && iMainWinScreenHeight == 480) {
            if (width < 640)
                ++width;
            if (height < 640)
                ++height;
        } else {
            if (destinationX > 0)
                --destinationX;
            if (destinationY > 0)
                --destinationY;
            if (width < 637)
                width += 4;
            if (height < 637)
                height += 4;
        }
    }

    if (gbLeaveNetBoxAlone != 0) {
        if (destinationY >= 411)
            return;
        if (destinationY + height >= 411)
            height = 411 - destinationY;
    }

    RECT invalidRect;
    invalidRect.left = destinationX * iMainWinScreenWidth / 640;
    invalidRect.top = destinationY * iMainWinScreenHeight / 480;
    invalidRect.right = (destinationX + width) * iMainWinScreenWidth / 640 - 1;
    invalidRect.bottom = (destinationY + height) * iMainWinScreenHeight / 480 - 1;

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
